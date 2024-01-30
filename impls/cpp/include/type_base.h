#ifndef TYPE_BASE_H
#define TYPE_BASE_H

#include <iostream>
#include <memory>
#include <string>


class ReferenceCounter {
public:
    ReferenceCounter() : m_count(size_t(0)) { }

    ReferenceCounter(const ReferenceCounter&) = delete;
    ReferenceCounter& operator=(const ReferenceCounter&) = delete;

    virtual ~ReferenceCounter() { }

    const ReferenceCounter* acquire() const { m_count++; return this; }
    size_t release() const { return --m_count; }
    size_t count() const { return m_count; }

private:
    mutable size_t m_count;
};

template<class T>
class RefCountedPtr {
public:
    RefCountedPtr() : m_object(0) { }

    RefCountedPtr(T* object) : m_object(0)
    {
        acquire(object);
    }

    RefCountedPtr(const RefCountedPtr& rhs) : m_object(0)
    {
        acquire(rhs.m_object);
    }

    ~RefCountedPtr() { release(); }

    const RefCountedPtr& operator=(const RefCountedPtr& rhs)
    {
        acquire(rhs.m_object);
        return *this;
    }

    bool operator == (const RefCountedPtr& rhs) const { return m_object == rhs.m_object; }
    bool operator != (const RefCountedPtr& rhs) const { return m_object != rhs.m_object; }

    operator bool() const { return m_object != NULL; }

    T* operator -> () const { return m_object; }
    T* ptr() const { return m_object; }

    friend std::ostream& operator<<(std::ostream& os, const RefCountedPtr& ptr)
    {
        os << ptr->toString();
        return os;
    }

private:
    void acquire(T* object)
    {
        if ( object != NULL ) {
            object->acquire();
        }

        release();
        m_object = object;
    }

    void release()
    {
        if ( (m_object != NULL) && (m_object->release() == 0) ) {
            delete m_object;
        }
    }

    T* m_object;
};

#endif // TYPE_BASE_H