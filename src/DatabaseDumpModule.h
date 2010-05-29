#ifndef _DATABASEDUMPMODULE_H
#define _DATABASEDUMPMODULE_H

class DBDumpModule
{
public:
	/** main handler
	 * return true if its handled by the DBDump module
	*/
	static bool HandleSpecialDump(FILE * fp, size_t deep, PyClass* object)
	{
		if (object->getbases() == NULL)
			return false;

		std::string className = object->getname()->content();

		// handling special classes so we have nice dumps
		/*if (className == "util.Rowset")
		{
			ClassDumper_util_rowset(fp, deep, (PyDict*)object->getbases());
			return true;
		}
		else if (className == "util.Row")
		{
			ClassDumper_util_row(fp, deep, (PyDict*)object->getbases());
			return true;
		}
		else */
		/*if (className == "objectCaching.CachedObject")
		{
			ClassDumper_dump_cache_csv(fp, deep, object->getbases());
			return false;
		}*/
		return false;
	}

private:
	static void ClassDumper_util_row(FILE * fp, size_t /*deep*/, PyDict* obj)
	{
		// should be a dict
		if (obj->gettype() != PyTypeDict)
		{
			ASCENT_HARDWARE_BREAKPOINT;
			return;
		}

		PyDict & book = *((PyDict*)obj);

		PyList & header = PyListRef_PyChameleon(book["header"]);

		fprintf(fp, "util.row:\n|");
		// the header should only contain strings

		// trick to make sexy row stuff
		std::vector<uint8> sexyRowSpacers;
		sexyRowSpacers.resize(header.size());
		int defaultRowWidth = 20;

		for (size_t i = 0; i < header.size(); i++)
		{
			PyChameleon & Itr = header[int(i)];
			assert(Itr.gettype() == PyTypeString);

			int ret = fprintf(fp, "%s", ((PyString&)Itr.getPyObjRef()).content());
			if (defaultRowWidth - ret > 0)
				WriteSpacer(fp, defaultRowWidth-ret);

			// safe the used row width so we can make the table sexy
			if (defaultRowWidth > ret)
				sexyRowSpacers[i] = uint8(defaultRowWidth);
			else
				sexyRowSpacers[i] = uint8(ret);

			fprintf(fp, "|");
		}
		fprintf(fp, "\n");

		PyList & line = PyListRef_PyChameleon(book["line"]);
		assert(line.gettype() == PyTypeList);

		fprintf(fp, "|");
		for (size_t j = 0; j < line.size(); j++)
		{
			size_t fsexypos = ftell(fp);

			// even more hacking..
			PyObject* obj = line[int(j)].getPyObject();
			if (obj->gettype() == PyTypeString)
			{
				if( ((PyString*)obj)->length() > sexyRowSpacers[j])
				{
					((PyString*)obj)->resize(sexyRowSpacers[j]-4);
					((PyString*)obj)->append("..");
				}
			}
			else if (obj->gettype() == PyTypeUnicode)
			{
				if( ((PyUnicodeUCS2*)obj)->length() >= sexyRowSpacers[j])
				{
					((PyUnicodeUCS2*)obj)->resize(sexyRowSpacers[j]-2);
				}
				else if (int(((PyUnicodeUCS2*)obj)->length()) == sexyRowSpacers[j]-1)
				{
					((PyUnicodeUCS2*)obj)->resize(sexyRowSpacers[j]-2);
				}

				printf("dump string size:%u\n", ((PyUnicodeUCS2*)obj)->length());
			}

			Dump(fp, line[int(j)].getPyObject(), size_t(-1), true);
			fsexypos = ftell(fp) - fsexypos;

			if (sexyRowSpacers[int(j)] > fsexypos)
				WriteSpacer(fp, sexyRowSpacers[int(j)]-fsexypos);

			fprintf(fp, "|");
		}
		fprintf(fp, "\n");

		fflush(fp);
	}

	static void ClassDumper_util_rowset(FILE * fp, size_t deep, PyDict* obj)
	{
		// should be a dict
		if (obj->gettype() != PyTypeDict)
		{
			assert(false);
			return;
		}

		PyDict & book = *((PyDict*)obj);

		PyList & header = PyListRef_PyChameleon(book["header"]);

		fprintf(fp, "util.rowset:\n|");
		// the header should only contain strings

		// trick to make sexy row stuff
		std::vector<uint8> sexyRowSpacers;
		sexyRowSpacers.resize(header.size());
		int defaultRowWidth = 20;

		for (size_t i = 0; i < header.size(); i++)
		{
			PyChameleon & Itr = header[int(i)];
			assert(Itr.gettype() == PyTypeString);

			int ret = fprintf(fp, "%s", ((PyString&)Itr.getPyObjRef()).content());
			if (defaultRowWidth - ret > 0)
				WriteSpacer(fp, defaultRowWidth-ret);

			// safe the used row width so we can make the table sexy
			if (defaultRowWidth > ret)
				sexyRowSpacers[i] = (uint8)defaultRowWidth;
			else
				sexyRowSpacers[i] = (uint8)ret;

			fprintf(fp, "|");
		}
		fprintf(fp, "\n");

		PyList & lines = PyListRef_PyChameleon(book["lines"]);

		for (size_t i = 0; i < lines.size(); i++)
		{
			PyList & Itr = (PyList&)lines[int(i)].getPyObjRef();
			assert(Itr.gettype() == PyTypeList);

			fprintf(fp, "|");
			for (size_t j = 0; j < Itr.size(); j++)
			{
				size_t fsexypos = ftell(fp);

				// even more hacking..
				PyObject* obj = Itr[int(i)].getPyObject();
				if (obj->gettype() == PyTypeString)
				{
					if( ((PyString*)obj)->length() > sexyRowSpacers[int(i)])
					{
						((PyString*)obj)->resize(sexyRowSpacers[int(i)]-4);
						((PyString*)obj)->append("..");
					}
				}
				else if (obj->gettype() == PyTypeUnicode)
				{
					if( ((PyUnicodeUCS2*)obj)->length() >= sexyRowSpacers[int(i)])
					{
						((PyUnicodeUCS2*)obj)->resize(sexyRowSpacers[int(i)]-2);
					}
					else if (int(((PyUnicodeUCS2*)obj)->length()) == sexyRowSpacers[int(i)]-1)
					{
						((PyUnicodeUCS2*)obj)->resize(sexyRowSpacers[int(i)]-2);
					}

					printf("dump string size:%u\n", ((PyUnicodeUCS2*)obj)->length());
				}

				Dump(fp, Itr[int(j)].getPyObject(), size_t(-1), true);
				fsexypos = ftell(fp) - fsexypos;

				if (sexyRowSpacers[int(j)] > fsexypos)
					WriteSpacer(fp, sexyRowSpacers[int(j)]-fsexypos);

				fprintf(fp, "|");
			}
			fprintf(fp, "\n");
		}

		PyObject * RowClass = (PyObject*)book["RowClass"].getPyObject();
		fprintf(fp, "RowClass:\n");
		Dump(fp, RowClass, deep);

		fflush(fp);
	}

	static void ClassDumper_cvs_util_crowset(FILE * fp, size_t deep, PyList* obj)
	{
		PyList::iterator itr = obj->begin();
		bool writtenHeader = false;

		for (; itr != obj->end(); itr++)
		{
			int virtualFieldIndex = 0;
			PyPackedRow *row = (PyPackedRow *)(*itr)->getPyObject();
			assert(row->gettype() == PyTypePackedRow);

			if (writtenHeader == false)
			{
				writtenHeader = true;
				PyClass * rowHeader = row->getheader();
				PyTuple * headerBase = rowHeader->getbases();
				headerBase = headerBase->GetItem_asPyTuple(1);
				headerBase = headerBase->GetItem_asPyTuple(0);
			
				for (int i = 0; i < (int)headerBase->size(); i++)
				{
					PyTuple * field = headerBase->GetItem_asPyTuple(i);
					assert(field->size() == 2);
					Dump(fp, field->GetItem(0), -1, true);
					fputs(",", fp);
				}
				fputc('\n', fp);
				continue; // skip the code that is below this...
			}

			PyTuple& fieldInfo = *((PyTuple*)row->getRawPayLoad());
			PyObject* fieldPayload = NULL;
			for(int i = 0; i < (int)fieldInfo.size(); i++)
			{
				fieldPayload = fieldInfo[i].getPyObject();
				if (fieldPayload == NULL)
				{
					fieldPayload = row->GetLeaf(virtualFieldIndex++);					
				}

				assert(fieldPayload);
				Dump(fp, fieldPayload, -1, true);
				fputs(",", fp);
			}
			fputc('\n', fp);
		}
	}

	static void ClassDumper_cvs_util_tuple(FILE * fp, size_t deep, PyObject* obj)
	{
		PyTuple* payload = (PyTuple*)obj;

		PyList & header = *payload->GetItem_asPyList(0);

		/* write csv header stuff */
		for (int j = 0; j < (int)header.size(); j++)
		{
			Dump(fp, header[j].getPyObject(), -1, true, true);
			fputs(",", fp);
		}
		fputc('\n', fp);

		/* write csv data */

		PyList & fields = *payload->GetItem_asPyList(1);

		for (int i = 0; i < (int)fields.size(); i++)
		{
			PyList & field = (PyList&)fields[i].getPyObjRef();
			for (int j = 0; j < (int)header.size(); j++)
			{
				Dump(fp, field[j].getPyObject(), -1, true, true);
				fputs(",", fp);
			}
			fputc('\n', fp);
		}
	}

	static void ClassDumper_dump_cache_csv(FILE * fp, size_t /*deep*/, PyTuple* obj)
	{
		ReadStream *rawsubstream = NULL;
		MarshalStream *substream = NULL;

		fprintf(fp, "cache file dumped to sql\n");

		/* do some basic checking */
		if (obj->gettype() != PyTypeTuple) { ASCENT_HARDWARE_BREAKPOINT; return; }
		assert(obj->size() == 7);
		
		/* check if object is part of a substream of part of a string (PyBuffer) */
		PyString * content = obj->GetItem_asPyString(4);
		PyObject* obj1 = NULL;

		if (content != NULL && content->content()[0] == 0x78)
		{
			rawsubstream = new ReadStream((const char*)content->content(), content->length());
			substream = new MarshalStream();
			obj1 = substream->load(*rawsubstream);
			if (obj1 == NULL)
			{
				ASCENT_HARDWARE_BREAKPOINT;
				return;
			}
		}
		else
		{
			PySubStream * subStream = obj->GetItem_asPySubStream(4);
			if (subStream != NULL)
			{
				rawsubstream = new ReadStream((const char*)subStream->content(), subStream->size());
				substream = new MarshalStream();
				obj1 = substream->load(*rawsubstream);
				if (obj1 == NULL)
				{
					ASCENT_HARDWARE_BREAKPOINT;
					return;
				}
			}
			else
			{
				Dump(stdout, (PyObject*)obj, 0);
				// this shouldn't happen.
				ASCENT_HARDWARE_BREAKPOINT;
				obj1 = (PyObject*)obj->GetItem_asPyTuple(4);
			}			
		}

		PyString * objectID = obj->GetItem_asPyString(6); // cache objectID
		std::string out_file_name = objectID->content();
		out_file_name+= ".csv";

		FILE * henk = fopen(out_file_name.c_str(), "wb");
		
		// handling different object formats
		if (obj1->gettype() == PyTypeTuple)
		{
			ClassDumper_cvs_util_tuple(henk, 0, obj1);
		}
		else if (obj1->gettype() == PyTypeList)
		{
			ClassDumper_cvs_util_crowset(henk, 0, (PyList*)obj1);
		}
		// handle class objects
		else if (obj1->gettype() == PyTypeClass)
		{
			PyClass * classObj = reinterpret_cast<PyClass*>(obj1);
			Dump(stdout, obj1, 0);
		}

		fclose(henk);

		SafeDelete(rawsubstream);
		SafeDelete(substream);
	}
};
#endif // _DATABASEDUMPMODULE_H
