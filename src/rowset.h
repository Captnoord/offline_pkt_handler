#include "PyObjectDumper.h"

class util_KeyVal : public PyClass
{
protected:
public:
    util_KeyVal() : PyClass( "util.KeyVal" ) {}

    ~util_KeyVal(){};
    void destruct() {}

    util_KeyVal* New()
    {
        return new util_KeyVal();
    }

    /* comments: format guessed from compiled scripts */
    bool init(PyObject* state)
    {
        if (state->gettype() != PyTypeDict)
            return false;
        
        // TODO check if this has any data in it... it should be empty
        mDict->DecRef();

        // replace the current dict with that one to init...
        mDict = (PyDict*)state;
        PyIncRef(state);
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    };
};

class util_Rowset : public PyClass
{
protected:
public:
    util_Rowset() : PyClass( "util.Rowset" ) {}

    ~util_Rowset(){};
    void destruct() {}

    util_Rowset* New()
    {
        return new util_Rowset();
    }

    /* comments: format guessed from compiled scripts */
    bool init(PyObject* state)
    {
        /*if (!PyTuple_Check(state))
            return false;
        PyTuple * pTuple = (PyTuple*)state;

        mDict->set_item("header", pTuple->GetItem(0));
        mDict->set_item("lines", pTuple->GetItem(1));
        mDict->set_item("RowClass", pTuple->GetItem(2));*/

        if (!PyDict_Check(state))
            return false;

        // TODO check if this has any data in it... it should be empty
        mDict->DecRef();

        // replace the current dict with that one to init...
        mDict = (PyDict*)state;
        PyIncRef(state);

        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    }

    // todo implement 'Row'
    //RowClass = Row
};
