//#pragma pack(push,1)

// this is pure evill bleh
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
    
    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        if (state->gettype() != PyTypeTuple)
            return false;

        PyTuple * pState = (PyTuple*)state;

        // TODO: add item count check...

        mDict->set_item("objectID", pState->GetItem(0));
        mDict->set_item("nodeID", pState->GetItem(1));
        mDict->set_item("objectVersion", pState->GetItem(2));
        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };
};

//#pragma pack(pop)