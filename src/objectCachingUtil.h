
/* this is done using pure evil bleh, it needs some more cleaning */
class util_CachedObject : public PyClass
{
protected:
public:
    util_CachedObject() : PyClass("util.CachedObject") {}

    ~util_CachedObject(){};
    void destruct() {}

    util_CachedObject* New()
    {
        return new util_CachedObject();
    }
    
    /* comments: format guessed from compiled scripts */
    bool init(PyObject* state)
    {
        if (!PyTuple_Check(state))
            return false;

        PyTuple * pState = (PyTuple*)state;

        if (pState->size() != 3)
            return false;

        mDict->set_item("objectID", pState->GetItem(0));
        mDict->set_item("nodeID", pState->GetItem(1));
        mDict->set_item("objectVersion", pState->GetItem(2));
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    }
};
