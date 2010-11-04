#include "PyObjectDumper.h"

//#pragma pack(push,1)

/*
def __str__(self):
    members = dict(filter(lambda (k, v,):(not k.startswith('__')), self.__dict__.items()))
    return ('%s %s: %s' % ((self.__doc__ or 'Anonymous'), self.__class__.__name__, members))
*/

class util_KeyVal : public PyClass
{
protected:
public:
    util_KeyVal() : PyClass()
    {
        setname(new PyString("util.KeyVal"));
    }
    ~util_KeyVal(){};
    void destruct() {}

    util_KeyVal* New()
    {
        return new util_KeyVal();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        if (state->gettype() != PyTypeDict)
            return false;
        
        // TODO check if this has any data in it... it should be empty
        mDict->DecRef();

        // replace the current dict with that one to init...
        mDict = (PyDict*)state; state->IncRef();
        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };
};

/*class Rowset:
    __module__ = __name__
    __guid__ = 'util.Rowset'
    __passbyvalue__ = 1
    __immutable__ = weakref.WeakKeyDictionary()
    RowClass = Row

    def __init__(self, hd = None, li = None, RowClass = Row):
        if (hd is None):
            hd = []
        if (li is None):
            li = []
        self.header = hd
        self.lines = li
        self.RowClass = RowClass
        */

class util_Rowset : public PyClass
{
protected:
public:
    util_Rowset() : PyClass()
    {
        setname(new PyString("util.Rowset"));
    }
    ~util_Rowset(){};
    void destruct() {}

    util_Rowset* New()
    {
        return new util_Rowset();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        /*if (state->gettype() != PyTypeTuple)
            return false;
        PyTuple * pTuple = (PyTuple*)state;

        mDict->set_item("header", pTuple->GetItem(0));
        mDict->set_item("lines", pTuple->GetItem(1));
        mDict->set_item("RowClass", pTuple->GetItem(2));*/

        if (state->gettype() != PyTypeDict)
            return false;

        // TODO check if this has any data in it... it should be empty
        mDict->DecRef();

        // replace the current dict with that one to init...
        mDict = (PyDict*)state; state->IncRef();

        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };

    // todo implement 'Row'
    //RowClass = Row
};

//#pragma pack(pop)