#ifndef PYCALLOBJECT_H
#define PYCALLOBJECT_H


/*  PyClass
    one of the way of communication is exchange states...
    so lets say we have a generic class which has variable called 'itemID'.
    we can use getstate and setstate to serialize and deserialize the class
    state.
    Often this is done by using tuple's containing only the data objects from the
    PyClass.
*/

/* move this to its own file */
class util_Row : public PyClass
{
public:
    util_Row() : PyClass( "util.Row" ) {}
    util_Row* New()
    {
        return new util_Row();
    }

    void destruct() {}

    /* this is a stub that needs to be implemented */
    int UpdateDict(PyObject* bases)
    {
        return -1;
    }

    /* this is a stub that needs to be implemented */
    bool init(PyObject* state)
    {
        return false;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    };
    
};

/** MachoAddress
 * @note move to its own file
    
    -A macho client tuple address       4 elements
    -A macho broadcast tuple address    4 elements
    -A macho any tuple address          3 elements
    -A macho node tuple address         4 elements

    For all address types addressType is the first element.

    -Macho client
        *addressType = theTuple[0]
        *clientID = theTuple[1]
        *callID = theTuple[2]
        *service = theTuple[3]
    -Macho broadcast
        *addressType = theTuple[0]
        *broadcastID = theTuple[1]
        *narrowcast = theTuple[2]
        *idtype = theTuple[3]
    -Macho any
        *addressType = theTuple[0]
        *service = theTuple[1]
        *callID = theTuple[2]
    -Macho node
        *addressType = theTuple[0]
        *nodeID = theTuple[1]
        *service = theTuple[2]
        *callID = theTuple[3]
*/

class macho_MachoAddress : public PyClass
{
public:
    macho_MachoAddress() : PyClass( "macho.MachoAddress" ) {}

    ~macho_MachoAddress(){}
    void destruct() {}

    macho_MachoAddress* New()
    {
        return new macho_MachoAddress();
    }

    /* this is a stub that needs to be implemented */
    int UpdateDict(PyObject* bases)
    {
        return -1;
    }

    // bleh this code causes the object to be shared.... we need to make set_item increase the object ref... so this gets a bit cleaner..

    // serialize this class
    PyTuple* GetState()
    {
        PyTuple* state = NULL;
        
        // this one is a bit crap.... we need to increase ref count?
        // maybe we need todo this if we do MT with shared objects between threads ( which is killing bleh )
        PyString* addressType = (PyString*)mDict->get_item("addressType");
        if (*addressType == "C")
        {
            state = new PyTuple(4);
            state->set_item(0, mDict->get_item("addressType"));
            state->set_item(1, mDict->get_item("clientID"));
            state->set_item(2, mDict->get_item("callID"));
            state->set_item(3, mDict->get_item("service"));
        }
        else if (*addressType == "B")
        {
            state = new PyTuple(4);
            state->set_item(0, mDict->get_item("addressType"));
            state->set_item(1, mDict->get_item("broadcastID"));
            state->set_item(2, mDict->get_item("narrowcast"));
            state->set_item(3, mDict->get_item("idtype"));
        }
        else if (*addressType == "A")
        {
            state = new PyTuple(3);
            state->set_item(0, mDict->get_item("addressType"));
            state->set_item(1, mDict->get_item("service"));
            state->set_item(2, mDict->get_item("callID"));
        }
        else if (*addressType == "N")
        {
            state = new PyTuple(4);
            state->set_item(0, mDict->get_item("addressType"));
            state->set_item(1, mDict->get_item("nodeID"));
            state->set_item(2, mDict->get_item("service"));
            state->set_item(3, mDict->get_item("callID"));
        }
        else
        {
            // make sure it crashes...
            ASCENT_HARDWARE_BREAKPOINT;
        }

        return state;
    }

    bool init(PyObject* state)
    {
        if (!PyTuple_Check(state))
            return false;

        PyTuple * pState = (PyTuple *)state;

        int addressType = pState->GetItem_asInt(0);
        if (addressType == 2)
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("clientID", pState->GetItem(1));
            mDict->set_item("callID", pState->GetItem(2));
            mDict->set_item("service", pState->GetItem(3));
        }
        else if (addressType == 4)
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("broadcastID", pState->GetItem(1));
            mDict->set_item("narrowcast", pState->GetItem(2));
            mDict->set_item("idtype", pState->GetItem(3));
        }
        else if (addressType == 8)
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("service", pState->GetItem(1));
            mDict->set_item("callID", pState->GetItem(2));
        }
        else if (addressType == 1)
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("nodeID", pState->GetItem(1));
            mDict->set_item("service", pState->GetItem(2));
            mDict->set_item("callID", pState->GetItem(3));
        }
        else
        {
            ASCENT_HARDWARE_BREAKPOINT;
            return false;
        }
        return true;
    }
};



class CallMgr : public Singleton<CallMgr>
{
public:
    CallMgr();
    ~CallMgr();

    bool reg(const char* name, PyClass* module);

    PyClass* find(const char* module);
    PyClass* find(PyString* module);

private:
    typedef std::map<std::string, PyClass*> CallMap;
    typedef CallMap::iterator CallMapItr;

    CallMap mCallMap;
};

#endif // PYCALLOBJECT_H

#define sPyCallMgr CallMgr::getSingleton()
