#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <functional>

// Subscriber
//////////////////////////

typedef uint64_t SubscriberId;
class CSubscriber
{
public:
    virtual ~CSubscriber() {}
    virtual void MessageHandler(void *pContext) = 0;
    virtual void UnsubscribeHandler() = 0;
    SubscriberId GetSubscriberId() { return (SubscriberId)this; }
};
// typedef  CSubscriberPtr;

class CDispatcherBase
{
public:
    CDispatcherBase(const CDispatcherBase &) = delete;
    void operator=(const CDispatcherBase &) = delete;
    CDispatcherBase() = default;

    virtual SubscriberId Subscribe(std::shared_ptr<CSubscriber> pNewSubscriber) = 0;
    virtual bool Unsubscribe(SubscriberId id) = 0;
};
// Dispatcher
/////////////////////////////////
class CDispatcher : virtual public CDispatcherBase
{
private:
    class CSubscriberItem
    {
    public:
        CSubscriberItem(std::shared_ptr<CSubscriber> pSubscriber)
            : m_pSubscriber(pSubscriber)
        {
        }
        ~CSubscriberItem()
        {
            m_pSubscriber->UnsubscribeHandler();
        };
        std::shared_ptr<CSubscriber> Subscriber() const { return m_pSubscriber; }

    private:
        std::shared_ptr<CSubscriber> m_pSubscriber;
    };
    typedef std::shared_ptr<CSubscriberItem> CSubscriberItemPtr;
    typedef std::vector<CSubscriberItemPtr> CSubscriberList;
    typedef std::shared_ptr<CSubscriberList> CSubscriberListPtr;

public:
    CDispatcher()
    {
    }

private:
    CDispatcher(const CDispatcher &) {}
    CDispatcher &operator=(const CDispatcher &) { return *this; }

public:
    SubscriberId Subscribe(std::shared_ptr<CSubscriber> pNewSubscriber)
    {
        // Declaration of the next shared pointer before ScopeLocker
        // prevents release of subscribers from under lock
        CSubscriberListPtr pNewSubscriberList(new CSubscriberList());
        // Enter to locked section
        std::scoped_lock<std::mutex> lock(m_Lock);
        if (m_pSubscriberList)
        {
            // Copy existing subscribers
            pNewSubscriberList->assign(m_pSubscriberList->begin(), m_pSubscriberList->end());
        }
        for (size_t i = 0; i < pNewSubscriberList->size(); ++i)
        {
            CSubscriberItemPtr pSubscriberItem = (*pNewSubscriberList)[i];
            if (pSubscriberItem->Subscriber()->GetSubscriberId() == pNewSubscriber->GetSubscriberId())
            {
                return 0;
            }
        }
        // Add new subscriber to new subscriber list
        pNewSubscriberList->push_back(CSubscriberItemPtr(new CSubscriberItem(pNewSubscriber)));
        // Exchange subscriber lists
        m_pSubscriberList = pNewSubscriberList;
        return pNewSubscriber->GetSubscriberId();
    }
    bool Unsubscribe(SubscriberId id)
    {
        // Declaration of the next shared pointers before ScopeLocker
        // prevents release of subscribers from under lock
        CSubscriberItemPtr pSubscriberItemToRelease;
        CSubscriberListPtr pNewSubscriberList;
        // Enter to locked section
        std::scoped_lock<std::mutex> lock(m_Lock);
        if (!m_pSubscriberList)
        {
            // No subscribers
            return false;
        }
        pNewSubscriberList = CSubscriberListPtr(new CSubscriberList());
        for (size_t i = 0; i < m_pSubscriberList->size(); ++i)
        {
            CSubscriberItemPtr pSubscriberItem = (*m_pSubscriberList)[i];
            if (pSubscriberItem->Subscriber()->GetSubscriberId() == id)
            {
                pSubscriberItemToRelease = pSubscriberItem;
            }
            else
            {
                pNewSubscriberList->push_back(pSubscriberItem);
            }
        }
        // Exchange subscriber lists
        m_pSubscriberList = pNewSubscriberList;
        if (!pSubscriberItemToRelease.get())
        {
            return false;
        }
        return true;
    }
    void SendMessageLP(void *pContext)
    {
        CSubscriberListPtr pSubscriberList;
        {
            std::scoped_lock<std::mutex> lock(m_Lock);
            if (!m_pSubscriberList)
            {
                // No subscribers
                return;
            }
            // Get shared pointer to an existing list of subscribers
            pSubscriberList = m_pSubscriberList;
        }
        // pSubscriberList pointer to copy of subscribers' list
        for (size_t i = 0; i < pSubscriberList->size(); ++i)
        {
            (*pSubscriberList)[i]->Subscriber()->MessageHandler(pContext);
        }
    }

private:
    CSubscriberListPtr m_pSubscriberList;
    std::mutex m_Lock;
};

// simple implement of CSubscriber
/////////////////////////////////
class CListener : virtual public CSubscriber
{
public:
    void SetMessageFunc(std::function<void(void *)> handler)
    {
        _handler = handler;
    }

    virtual void MessageHandler(void *pContext)
    {
        // Эмулируем блокирующий вызов SendMessage
        std::scoped_lock<std::mutex> lock(m_Lock);
        if (_handler != nullptr)
            _handler(pContext);
    }

    virtual void UnsubscribeHandler() {

    };

private:
    std::mutex m_Lock;
    std::function<void(void *)> _handler = NULL;
};