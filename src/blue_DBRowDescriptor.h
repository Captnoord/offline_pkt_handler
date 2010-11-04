//blue.DBRowDescriptor

//#pragma pack(push,1)

class blue_DBRowDescriptor : public PyClass
{
protected:
public:
    blue_DBRowDescriptor() : PyClass()
    {
        setname(new PyString("blue.DBRowDescriptor"));
        blue_obj = true; // bleh hack...
    }
    ~blue_DBRowDescriptor()
    {
    };

    void destruct() {}

    blue_DBRowDescriptor* New()
    {
        return new blue_DBRowDescriptor();
    }

    // comments: format guessed from compiled scripts
    bool init(PyObject* state)
    {
        // TODO split Class and CClass
        if (state->gettype() == PyTypeClass)
        {
            // not implemented yet
            ASCENT_HARDWARE_BREAKPOINT;
        }
        else
        {
            // TODO add sub type check..
            if (state->gettype() == PyTypeTuple)
            {
                //ASCENT_HARDWARE_BREAKPOINT; // this is kinda correct... it needs a hell lot more... can't explain... need to reverse..
                //DumpObject(stdout, state);
                PyTuple* pTuple = (PyTuple*)state;
                PyTuple* pSubTuple = (PyTuple*)pTuple->GetItem(0);
                for (int i = 0; i < pSubTuple->size(); i++)
                {
                    PyTuple* itemTuple = (PyTuple*)pSubTuple->GetItem(i);
                    assert(itemTuple);
                    
                    // bleh this is evil... aka fucked up..
                    mTableNames.push_back(itemTuple->GetItem_asPyString(0)->content());
                    mTableTypes.push_back(itemTuple->GetItem_asInt(1));
                    mTableSizes.push_back(1); // dirty hack I am surely gonna forget... we haven't implemented the shit for gettypesize..
                }
            }
            else
            {
                sLog.Error("DBRowDescriptor", "expected tuple of tuples");
                ASCENT_HARDWARE_BREAKPOINT;
                return false;
            }
        }

        return true;
    }

    PyTuple* GetState()
    {
        return NULL;
    };

    // this is really is CPythonClass
    bool blue_obj;

    // clean this up so it only contains the stuff we need... more compact
    std::vector <std::string>   mTableNames;
    std::vector <int32>         mTableTypes;
    std::vector <uint32>        mTableSizes;
};

//#pragma pack(pop)