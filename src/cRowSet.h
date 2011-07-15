#include "PyObjectDumper.h"

//#pragma pack(push,1)

class dbutil_CRowset : public PyClass
{
protected:
public:
    dbutil_CRowset() : PyClass( "dbutil.CRowset" ), mList( NULL )
    {
        if (mList != NULL)
            PyDecRef(mList);
        mList = new PyList();
    }

    // this of course will never be called
    ~dbutil_CRowset()
    {
    
    };

    void destruct()
    {
        mList->DecRef();
    }

    dbutil_CRowset* New()
    {
        return new dbutil_CRowset();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        /*
        def __init__(self, header, rows):
        list.__init__(self, rows)
        self.header = header
        */

        PyTuple * pTuple = (PyTuple *)state;

        mList->init(pTuple->GetItem(1));
        mDict->set_item("header", pTuple->GetItem(0));


        /*if (state->gettype() != PyTypeTuple)
        return false;
        PyTuple * pTuple = (PyTuple*)state;

        mDict->set_item("header", pTuple->GetItem(0));
        mDict->set_item("lines", pTuple->GetItem(1));
        mDict->set_item("RowClass", pTuple->GetItem(2));*/

        //if (state->gettype() != PyTypeDict)
          //  return false;

        // TODO check if this has any data in it... it should be empty
        //mDict->DecRef();

        // replace the current dict with that one to init...
        //mDict = (PyDict*)state; state->IncRef();

        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };

    // this object derives from a PyList.... we hack it this way for now...
    PyList * mList;
};

//#pragma pack(pop)