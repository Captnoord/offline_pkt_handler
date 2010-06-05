#ifndef machoNetPacket_h__
#define machoNetPacket_h__

#pragma pack(push,1)

class MachoPacket : public PyClass
{
protected:
public:
    MachoPacket() : PyClass() {}
    
    ~MachoPacket(){};
    virtual MachoPacket* New() = 0;


    bool setattr(std::string attr_name, PyObject * attr_obj)
    {
        /*
        if hasattr(self, 'thePickle'):
        if hasattr(self, attr):
        curr = getattr(self, attr)
        if ((type(curr) not in self.__intorstringtype__) or ((type(value) not in self.__intorstringtype__) or (curr != value))):
        self.Changed()
        else:
        self.Changed()
        self.__dict__[attr] = value
        */

        return mDict->set_item(attr_name.c_str(), attr_obj);
    }

    bool init(PyObject* state)
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
            mDict->set_item("channel", oob->get_item("channel", (PyObject*)new PyBaseNone())); // default would be PyNone
            mDict->set_item("compressedPart", oob->get_item("compressedPart", (PyObject*)new PyInt(0))); // default would be PyInt(0);
        }

        if (params.size() != 0)
        {
            PyTuple* body = (PyTuple*)mDict->get_item("body");

            assert(params.size() == body->size());

            for (int i = 0; i < params.size(); i++)
            {
                /*for i in range(l):
                if params[i].endswith('?'):
                tmp = params[i][:-1]
                else:
                tmp = params[i]
                setattr(self, tmp, body[i])*/

                std::string tmp = params[i];

                setattr(tmp, body->GetItem(i));
            }
        }
        return false;
    }

    // not implemented for now
    PyTuple* GetState()
    {
        return NULL;
    };
protected:
    uint8 command;          // hmmm should we use normal types for this
    std::vector<std::string> params;     // should we use normal types for this
};

class macho_CallReq : public MachoPacket
{
public:
    macho_CallReq() : MachoPacket()
    {
        setname(new PyString("macho.CallReq"));
        params.push_back("payload");
        params.push_back("channel");
    }

    ~macho_CallReq()
    {

    }

    macho_CallReq* New()
    {
        return new macho_CallReq();
    }
};

class macho_CallRsp : public MachoPacket
{
public:
    macho_CallRsp() : MachoPacket()
    {
        setname(new PyString("macho.CallRsp"));
        params.push_back("payload");
    }

    ~macho_CallRsp(){}

    macho_CallRsp* New()
    {
        return new macho_CallRsp();
    }
};

class macho_SessionChangeNotification : public MachoPacket
{
public:
    macho_SessionChangeNotification() : MachoPacket()
    {
        setname(new PyString("macho.SessionChangeNotification"));
        params.push_back("change");
        params.push_back("nodesOfInterest");
    }

    ~macho_SessionChangeNotification(){}

    macho_SessionChangeNotification* New()
    {
        return new macho_SessionChangeNotification();
    }
};

class macho_SessionInitialStateNotification : public MachoPacket
{
public:
    macho_SessionInitialStateNotification() : MachoPacket()
    {
        setname(new PyString("macho.SessionInitialStateNotification"));
        params.push_back("initialstate");
    }

    ~macho_SessionInitialStateNotification(){}

    macho_SessionInitialStateNotification* New()
    {
        return new macho_SessionInitialStateNotification();
    }
};

class macho_PingRsp : public MachoPacket
{
public:
    macho_PingRsp() : MachoPacket()
    {
        setname(new PyString("macho.PingRsp"));
        params.push_back("times");
    }

    ~macho_PingRsp(){}

    macho_PingRsp* New()
    {
        return new macho_PingRsp();
    }
};

class macho_PingReq : public MachoPacket
{
public:
    macho_PingReq() : MachoPacket()
    {
        setname(new PyString("macho.PingReq"));
        params.push_back("times");
    }

    ~macho_PingReq(){}

    macho_PingReq* New()
    {
        return new macho_PingReq();
    }
};

class macho_ErrorResponse : public MachoPacket
{
public:
    macho_ErrorResponse() : MachoPacket()
    {
        setname(new PyString("macho.ErrorResponse"));
        params.push_back("originalCommand");
        params.push_back("code");
        params.push_back("payload");
    }

    ~macho_ErrorResponse(){}

    macho_ErrorResponse* New()
    {
        return new macho_ErrorResponse();
    }
};

class macho_Notification : public MachoPacket
{
public:
    macho_Notification() : MachoPacket()
    {
        setname(new PyString("macho.Notification"));
        params.push_back("payload");
        params.push_back("channel");
    }

    ~macho_Notification(){}

    macho_Notification* New()
    {
        return new macho_Notification();
    }
};

#pragma pack(pop)

#endif // machoNetPacket_h__