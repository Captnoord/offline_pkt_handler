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

//macho.CallReq
/*class CallReq : public PyClass
{
public:
    CallReq() : PyClass()
    {

    }
};*/

/* dono what todo with this one... yet */
class util_Row : public PyClass
{
public:
    util_Row() : PyClass()
    {
        setname(new PyString("util.Row"));
    }
    util_Row* New()
    {
        return new util_Row();
    }

    int UpdateDict(PyObject* bases)
    {
        return -1;
    }

    // stubs
    bool init(PyObject* state){return false;}
    PyTuple* GetState(){ return NULL;};
    
};


#if 0
//macho.CallReq

class macho_CallReq : public PyClass
{
public:
    macho_CallReq() : PyClass()
    {
        setname(new PyString("macho.CallReq"));
        mDict = new PyDict();
    }

    macho_CallReq* New()
    {
        return new macho_CallReq();
    }

    int UpdateDict(PyObject* bases)
    {
        return -1;
    }

    bool SetState(PyObject* state)
    {
        // this is generic for all packet types...
        /*
        (self.command, self.source, self.destination, self.userID, body, self.oob,) = state
        if (self.oob is None):
            self.oob = {}
        (self.channel, self.compressedPart,) = (self.oob.get('channel', None), self.oob.get('compressedPart', 0))
            params = self.__machodesc__['params']
        l = len(params)
        if (len(body) < l):
            l = len(body)
        for i in range(l):
        if params[i].endswith('?'):
        tmp = params[i][:-1]
        else:
        tmp = params[i]
        setattr(self, tmp, body[i])
        */

        if (state->gettype() != PyTypeTuple)
            return false;

        PyTuple * pState = (PyTuple *)state;

        mDict->set_item("command", pState->GetItem(0));
        mDict->set_item("source", pState->GetItem(1));
        mDict->set_item("destination", pState->GetItem(2));
        mDict->set_item("userID", pState->GetItem(3));
        mDict->set_item("body", pState->GetItem(4)); // this one isn't "self.body"
        mDict->set_item("oob", pState->GetItem(5));

        // we possible could get this from 
        PyDict * oob = (PyDict *)mDict->get_item("oob");

        if (oob->gettype() == PyTypeDict)
        {
            //mDict->set_item("channel", oob->get_item("channel")); // default would be PyNone
            mDict->set_item("compressedPart", oob->get_item("compressedPart")); // default would be PyInt(0);
        }

        // PyClass related
        /*
        class CallReq(MachoPacket):
        __module__ = __name__
        __guid__ = 'macho.CallReq'
        __machodesc__ = {'command': MACHONETMSG_TYPE_CALL_REQ,
        'params': ['payload',
        'channel?'],
        'response': CallRsp}
        */

        PyTuple * body = (PyTuple *)mDict->get_item("body");
        assert(body->size() == 2); // should have 2 elements

        mDict->set_item("payload", body->GetItem(0));
        mDict->set_item("channel", body->GetItem(1));
        return false;
    }

    // not implemented for now
    PyTuple* GetState()
    {
        return NULL;
    };
};

#endif

/*
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
#pragma pack(push,1)
class macho_MachoAddress : public PyClass
{
public:
    macho_MachoAddress() : PyClass()
    {
        setname(new PyString("macho.MachoAddress"));
    }

    ~macho_MachoAddress(){}

    macho_MachoAddress* New()
    {
        return new macho_MachoAddress();
    }

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
        if (state->gettype() != PyTypeTuple)
            return false;

        PyTuple * pState = (PyTuple *)state;

        // we need to inc ref of the addressType?
        PyString *addressType = pState->GetItem_asPyString(0);
        if (*addressType == "C")
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("clientID", pState->GetItem(1));
            mDict->set_item("callID", pState->GetItem(2));
            mDict->set_item("service", pState->GetItem(3));
        }
        else if (*addressType == "B")
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("broadcastID", pState->GetItem(1));
            mDict->set_item("narrowcast", pState->GetItem(2));
            mDict->set_item("idtype", pState->GetItem(3));
        }
        else if (*addressType == "A")
        {
            mDict->set_item("addressType", pState->GetItem(0));
            mDict->set_item("service", pState->GetItem(1));
            mDict->set_item("callID", pState->GetItem(2));
        }
        else if (*addressType == "N")
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

#pragma pack(pop)

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
