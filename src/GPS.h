
class exceptions_GPSTransportClosed : public PyClass
{
protected:
public:
    exceptions_GPSTransportClosed() : PyClass("exceptions.GPSTransportClosed") {}

    ~exceptions_GPSTransportClosed(){};
    void destruct() {}

    exceptions_GPSTransportClosed* New()
    {
        return new exceptions_GPSTransportClosed();
    }

    /* comments: format guessed from compiled scripts
     * @note not finished yet
     */
    bool init(PyObject* state)
    {
        if (!PyTuple_Check(state))
            return false;

        PyTuple * pState = (PyTuple*)state;

        // TODO: add item count check...

        /*mDict->set_item("reason", pState->GetItem(0));
        mDict->set_item("reasonCode", pState->GetItem(1));
        mDict->set_item("reasonArgs", pState->GetItem(2));
        */

        /*args = {'reason': getattr(self, 'reason', None),
            'reasonCode': getattr(self, 'reasonCode', None),
            'reasonArgs': getattr(self, 'reasonArgs', None),
            'exception': self}*/
        return true;
    }

    /* this is a stub that needs to be implemented */
    PyTuple* GetState()
    {
        return NULL;
    };
};
