// SampleBase.cpp: implementation of the SampleBase class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include <algorithm>
#include <iostream>

#include <assert.h>

#include <AttributeKeys.h>
#include <CMclAutoLock.h>
#include <MappedMemory.h>
#include <XScanResults.h>
#include <XAnalysisState.h>

#include "SampleBase.h"

#include "VQuarantineServer.h"
#include "GlobalData.h"
#include "ScannerSubmittor.h"
#include "Logger.h"

#include <qsFields.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

string  SampleBase::GenErrorMsg(HRESULT hr, const char* where, ulong id)
{
  std::string errMsg(where);
  errMsg  += " [";
  char    buffer[64];
  sprintf(buffer, "COM error = %x, sample id = %x", hr, id);
  errMsg  += buffer;
  errMsg  += "]";
  if (E_INVALIDARG == hr)
    errMsg += " E_INVALIDARG";


  return errMsg;
}



SampleBase::SampleBase()  : active(false), id(NULL), priority(0),
              gatewayCookie(""),
              status(SampleStatus::unknown), 
              finalStatus(false), sigSeqNum(0),
              attributes(""), changes(0), 
              fileHandle(INVALID_HANDLE_VALUE),
              copyItemData(NULL)
{
  EntryExit entryExit(Logger::LogIcePackSample, "SampleBase:: constructor");
}

SampleBase::SampleBase(const SampleBase& cpy) : active(false),
                        id(NULL),
                        itemKey(cpy.itemKey),
                        priority(cpy.priority),
                        forwardTime(cpy.forwardTime),
                        gatewayCookie(cpy.gatewayCookie),
                        status(cpy.status),
                        finalStatus(cpy.finalStatus),
                        sigSeqNum(cpy.sigSeqNum),
                        attributes(cpy.attributes),
                        changes(cpy.changes),
                        fileHandle(INVALID_HANDLE_VALUE),
                        copyItemData(NULL)
{
  EntryExit entryExit(Logger::LogIcePackSample, "SampleBase:: copy constructor");
}

SampleBase& SampleBase::operator=(const SampleBase& cpy)
{
  EntryExit entryExit(Logger::LogIcePackSample, "SampleBase::operator=");

  if (this == &cpy)
    return *this;

  Close();
  DeActivate();

//  id        = NULL;
//  active      = false;

  priority    = cpy.priority;
  forwardTime   = cpy.forwardTime;
  gatewayCookie = cpy.gatewayCookie;
  status      = cpy.status;
  finalStatus   = cpy.finalStatus;
  sigSeqNum   = cpy.sigSeqNum;
  attributes    = cpy.attributes;
  changes     = cpy.changes;
  fileHandle    = INVALID_HANDLE_VALUE;
  copyItemData  = NULL;

  itemKey.Clear();
  itemKey     = cpy.itemKey;

  return *this;
}

SampleBase::~SampleBase()
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::~SampleBase");

//  CMclAutoLock  lock(inUse);

  try
  {
    Close();

    if (id)
    {
      HRESULT hr = id->Release();
      if (FAILED(hr))
      {
        throw IcePackException(IcePackException::SampleBase,
              GenErrorMsg(hr, "~SampleBase, id->Release()", itemKey.lVal));
      }
    }
  }
  catch (...)
  {
    Logger::Log(Logger::LogError, Logger::LogIcePackSample,
          "~SampleBase, either id is invalid or Release threw an exception");
  }

  id        = NULL;
  active      = false;
  itemKey.lVal  = 0;
}

bool SampleBase::Open(void)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::Open");
//  CMclAutoLock  lock(inUse);

  Close();

  if (!copyItemData)
    copyItemData = GetICopyItemData();


  HRESULT hr = copyItemData->Open(itemKey.lVal);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "SampleBase::Open, copyItemData->Open", itemKey.lVal));
  }

  return true;
}

bool SampleBase::Size(ulong& size)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::Size");

//  CMclAutoLock  lock(inUse);

  if (!copyItemData)
    Open();

  size = 0;
  HRESULT hr = copyItemData->GetSize(&size);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::Size, copyItemData->GetSize", itemKey.lVal));
  }

  return true;
}

bool SampleBase::Read(ulong inSize, ulong& outSize, char* buffer)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::Read");
//  CMclAutoLock  lock(inUse);

  if (!copyItemData)
    Open();

  HRESULT hr = copyItemData->Read(inSize, &outSize, (uchar *) buffer);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::Read, copyItemData->Read", itemKey.lVal));
  }

  return true;
}

bool SampleBase::Close(void)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::~Close");
//  CMclAutoLock  lock(inUse);

  if (copyItemData)
  {
    HRESULT hr = copyItemData->Close();
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "SampleBase::Close, copyItemData->Close", itemKey.lVal));
    }

    hr = copyItemData->Release();
    copyItemData = NULL;

    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "SampleBase::Close, copyItemData->Release", itemKey.lVal));
    }
  }

  return true;
}

void SampleBase::ConvertToBSTR(VARIANT& vValue)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::ConvertToBSTR");

  if (VT_DATE == vValue.vt)
  {
    char    dateTime[64];
    SYSTEMTIME  st;

    dateTime[0] = '\0';
    if (0.0 != vValue.date)
    {
      VariantTimeToSystemTime(vValue.date, &st);
      GetDateFormat(NULL, 0, &st, "ddd, dd MMM yyyy", dateTime, sizeof(dateTime));
      int     len = strlen(dateTime);
      dateTime[len++] = ' ';
      GetTimeFormat(NULL, 0, &st, "HH:mm:ss GMT", &dateTime[len], sizeof(dateTime)-len);
    }

    _variant_t  vvValue(dateTime);
    VariantCopy(&vValue, &vvValue);
  }
  else if (VT_BSTR == vValue.vt)
  {
    /*
     * This code has been added to force IcePack to consider string-type
     * attributes to be NULL (that is to say, deleted) if they consist
     * of a lone space (awww!).
     */

    /*
     * Get the string value out.
     */
    _bstr_t val = vValue.bstrVal;

    /*
     * If it's a space, replace it with a new string value containing
     * NULL (this doesn't change the in-sample attributes, only the
     * in-memory one...).
     */
    if (! strcmp((char *)val, " "))
    {
      _variant_t vvValue("");
      VariantCopy(&vValue, &vvValue);
    }
  }
  else
  {
    if (VT_UI4 == vValue.vt && 0xFFFFFFFF == vValue.lVal)
    {
      _variant_t  vvValue("");
      VariantCopy(&vValue, &vvValue);
    }
    else
    {
      HRESULT hr = VariantChangeType(&vValue, &vValue, 0, VT_BSTR);
      if (FAILED(hr))
      {
        throw IcePackException(IcePackException::SampleBase, 
              GenErrorMsg(hr, "SampleBase::ConvertToBSTR, VariantChangeType", itemKey.lVal));
      }
    }
  }
}


bool SampleBase::GetAttribute(const char* key, std::string& value)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::GetAttribute");
//  CMclAutoLock  lock(inUse);
  char      buffer[256];

  ID();
  if (!active)
    throw IcePackException(IcePackException::SampleBase,
                GenErrorMsg(0, "SampleBase::GetAttribute, unable to activate sample", itemKey.lVal));

  strncpy(buffer, key, 128);

  _bstr_t   bKey(buffer);
  VARIANT   vValue;
  VariantInit(&vValue);

  HRESULT hr = id->GetValue(bKey, &vValue);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase, 
          GenErrorMsg(hr, "SampleBase::GetAttribute, id->GetValue", itemKey.lVal));
  }


  ConvertToBSTR(vValue);

  _bstr_t vvValue(vValue.bstrVal);
  value = vvValue;
  
  VariantClear(&vValue);

  return true;
}

bool SampleBase::GetAllAttributes(std::string& attr)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::GetAllAttributes");
  attr = "";

  ID();
  if (!active)
    throw IcePackException(IcePackException::SampleBase,
                GenErrorMsg(0, "SampleBase::GetAllAttributes, unable to activate sample",
                      itemKey.lVal));

//  CMclAutoLock  lock(inUse);
  VARIANT     variant;
  VariantInit(&variant);

  HRESULT hr = id->GetAllValues(&variant);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::GetAllAttributes, id->GetAllValues", itemKey.lVal));
  }
  
  SAFEARRAY *psa = variant.parray;
  SafeArrayLock(psa);
  VARIANT   *value;
  VARIANT   *element;
  _bstr_t   bStr;
  long    index[2];
  string    key;
  std::string stdbStr;
  int         escapeNow;

  for (uint i = 0; i < psa->rgsabound[0].cElements; i++)
  {
    index[0]  = i;
    index[1]  = 0;
    SafeArrayPtrOfIndex(psa, index, reinterpret_cast<void **>(&element));
    bStr    = element->bstrVal;
    stdbStr = bStr;
    if (AttributeKeys::ScanResult()   == stdbStr)
      continue;
    else if (0 == stdbStr.length())
      continue;
    else if (stdbStr.find("X-Backup-") == 0)
      continue;
    else
      key   = bStr;

    index[1]  = 1;
    SafeArrayPtrOfIndex(psa, index, reinterpret_cast<void **>(&value));
    ConvertToBSTR(*value);

    /*
     * Easier to do this with the value in a std::string, but
     * can't get there directly from value->bstrVal, for
     * some tedious reason related to constructors.
     */
    bStr    = value->bstrVal;
    stdbStr = bStr;

    /*
     * Now, take a look at the string for the purpose of removing
     * characters we don't like the look of.  Currently, this only
     * looks at the start of the string, but it could be enhanced
     * later to look for dubious characters across the length of
     * the string,
     */
    escapeNow = 0;
    while ((!escapeNow) && (stdbStr.length() > 0))
    {
      switch (stdbStr[0])
      {
        case ' '  :
        case '\t' :
        case '\r' :
        case '\n' :
          stdbStr.erase(0, 1);
          break;
        default :
          escapeNow = 1;
          break;
      }
    }

    /*
     * Now, if what's left has zero length, then ignore it.
     * Otherwise, construct an HTTP-stylie attribute for it.
     */
    if (stdbStr.length() > 0)
    {
      attr  += key;
      attr  += ": ";
      attr  += stdbStr;
      attr  += "\n";
    }

    ::VariantClear(value);
  }

  SafeArrayUnlock(psa);
  SafeArrayDestroy(psa);

//  VariantClear(&variant);

  return true;
}

bool SampleBase::SetAttribute(const char* key, const char* value)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::SetAttribute");
//  CMclAutoLock  lock(inUse);
  char      buffer[128];

  if ('\0' == value[0])
  {
    sprintf(buffer, "SampleBase::SetAttribute, ignoring request to set value of %s attribute to a null string", key);
    Logger::Log(Logger::LogInfo, Logger::LogIcePackSample, buffer);
    return true;
  }

  ID();
  if (!active)
    throw IcePackException(IcePackException::SampleBase,
                GenErrorMsg(0, "SampleBase::SetAttribute, unable to activate sample", itemKey.lVal));

  strncpy(buffer, key, 128);

  _bstr_t   bKey(buffer);
  _variant_t  vValue(value);

  HRESULT hr  = S_OK;
  if (AttributeKeys::DateCreated() == key     ||
    AttributeKeys::DateModified()== key     ||
    AttributeKeys::DateAccessed()== key     ||
    AttributeKeys::DateSampleStatus()== key     ||
//    AttributeKeys::DateQuarantineDate()== key ||
    AttributeKeys::DateSubmitted()== key    )
  {
//    hr = VariantChangeType(&vValue, &vValue, 0, VT_DATE);
    hr = 0;
    DateTime  dt(value);
    double    vTime(dt.AsOLEDouble());
    vValue.vt = VT_DATE;
    vValue.date = vTime;
  }
  else if (AttributeKeys::ScanVirusID()     == key  ||
       AttributeKeys::SampleStatus()      == key  ||
       AttributeKeys::SampleSubmissionRoute() == key  ||
       AttributeKeys::SamplePriority()    == key  ||
       AttributeKeys::ScanSignaturesSequence()== key  ||
       AttributeKeys::SampleFileSize()    == key  )
//       AttributeKeys::SampleFileID()      == key  )
  {
    hr = VariantChangeType(&vValue, &vValue, 0, VT_UI4);
  }

  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::SetAttribute, VariantChangeType", itemKey.lVal));
  }


  hr = id->SetValue(bKey, vValue);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase, 
          GenErrorMsg(hr, "SampleBase::SetAttribute, id->SetValue", itemKey.lVal));
  }

//  VariantClear(&vValue);

  return true;
}

bool  SampleBase::Commit()
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::Commit");

  ID();
  if (active)
  {
    HRESULT hr = id->Commit();
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase, 
            GenErrorMsg(hr, "SampleBase::Commit", itemKey.lVal));
    }
  }
  else
    throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(0, "SampleBase::Commit, unable to activate", itemKey.lVal));

  return true;
}

IQuarantineServer* SampleBase::GetIQS() const
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::GetIQS");
  IQuarantineServer *qs = NULL;

  HRESULT hr = CoCreateInstance(__uuidof(QuarantineServer), //CLSID_QuarantineServer,
                  NULL, CLSCTX_LOCAL_SERVER,
                  __uuidof(IQuarantineServer),
                  reinterpret_cast<void **>(&qs));

  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase, 
          GenErrorMsg(hr, "SampleBase::GetIQS [CoCreateInstance]", itemKey.lVal));
  }

  return qs;
}

ICopyItemData* SampleBase::GetICopyItemData()
{
  EntryExit     entryExit(Logger::LogIcePackSample, "SampleBase::GetICopyItemData");

  IQuarantineServer *qs = GetIQS();
  ICopyItemData*    iCopyItemData = NULL;

  HRESULT hr = qs->GetCopyInterface(&iCopyItemData);
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::GetICopyItemData, qs->GetCopyInterface", itemKey.lVal));
  }

  hr = qs->Release();
  if (FAILED(hr))
  {
    throw IcePackException(IcePackException::SampleBase,
          GenErrorMsg(hr, "SampleBase::GetICopyItemData, qs->Release", itemKey.lVal));
  }

  return iCopyItemData;
}

bool SampleBase::ID(IQuarantineServerItem* item)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::ID");

  Close();

  if (id)
  {
    HRESULT hr = id->Release();
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "ID(), id->Release()", itemKey.lVal));
    }
  }

  id    = item;

  if (NULL != id)
  {
    CComBSTR  key(QSERVER_ITEM_INFO_FILEID);

    HRESULT hr = id->GetValue(key, &itemKey);
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "SampleBase::ID, id->GetValue", itemKey.lVal));
    }

    assert(VT_UI4 == itemKey.vt);

    active = true;
  }
  else
    active = false;

  Reset();

  return true;
}

bool SampleBase::Key(ulong key)
{
  EntryExit   entryExit(Logger::LogIcePackSample, "SampleBase::ID");

  Close();

  if (id)
  {
    HRESULT hr = id->Release();
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
            GenErrorMsg(hr, "Key(), id->Release()", itemKey.lVal));
    }
  }

  itemKey.ulVal = key;

  active = false;

  ID();
  if (active)
  {
    Reset();
    return true;
  }
  else
    return false;
}
void SampleBase::DeActivate()
{
  if (active)
  {
    active = false;
    HRESULT hr = id->Release();
    if (FAILED(hr))
    {
      throw IcePackException(IcePackException::SampleBase,
                  GenErrorMsg(hr, "DeActivate(), id->Release()", itemKey.lVal));
    }
    id = NULL;
  }
}

IQuarantineServerItem* SampleBase::ID()
{
  if (!active)
  {
    IQuarantineServer*  iqs = GetIQS();
    HRESULT       hr = iqs->GetQuarantineItem(itemKey.lVal, &id);
    if (FAILED(hr))
    {
      id = NULL;
    }
    else
      active = true;

    iqs->Release();
  }

  return id;
}



bool CompSampleByKeyOnly::operator()(SampleBase& sample)
{
//  char  buffer[64];
//  sprintf(buffer, "CompSampleByIDOnly::operator(), id = %x, sample.ID = %lx",
//          id, sample.SampleKey());
//  Logger::Log(Logger::LogDebug, Logger::LogIcePackSample, buffer);

  return key == (uint) sample.SampleKey();
}

/* ----- */

static bool turnABlindEyeToThisAttribute(std::string &attributeName)
{
  /*
   * Ignore anything that doesn't start X-.
   */
  if (0 != attributeName.find("X-"))
    return true;

  if (AttributeKeys::SubmissionCount() == attributeName)
    return true;

  if (AttributeKeys::SampleChanges() == attributeName)
    return true;

  if (AttributeKeys::AlertStatusTimer() == attributeName)
    return true;

  /*
   * If the attribute name starts with X-Backup-, then we should
   * ignore it here.
   */
  if (0 == attributeName.find("X-Backup-"))
    return true;

  return false;
}

/* ----- */

static bool isThisAScanAttribute(std::string &attributeName)
{
  return ((0 == attributeName.find("X-Scan")) ? true : false);
}

/* ----- */

/*
 * This function allows the caller to work out whether or not
 * an attribute is 'deleted'.  Due to the inexplicable lack
 * of a delete method for attributes on the Quarantine interfaces,
 * there are instead some magic (agreed) values meaning 'deleted'.
 *
 *
 * This function will return true if the attribute is deleted,
 * and false if it is not.
 */
bool isThisAttributePseudoDeleted(VARIANT &value)
{
  _bstr_t  bstrVal;
  char    *strVal;

  switch (value.vt)
  {
    case VT_UI4 :
      return (0xFFFFFFFF == value.lVal) ? true : false;
      break;
    case VT_DATE :
      return (0.0 == value.date) ? true : false;
      break;
    case VT_BSTR :
      bstrVal = value.bstrVal;
      strVal  = (char *)bstrVal;
      return ((0 == strlen(strVal)) || (! strcmp(strVal, " "))) ? true : false;
      break;
    default :
      /* DO NOTHING */
      break;
  }

  /*
   * Mysterious attribute type, so presumably not deleted.
   */
  return false;
}

/* ----- */

/*
 * This method is used internally to the monitor, and is called
 * as soon as IcePack notices that a new sample has appeared
 * in central quarantine.
 *
 * Its purpose is to backup the client attributes.  This takes
 * two forms -- the creation of a whole bunch of X-Backup...
 * attributes, and the copying of the X-Scan... attributes
 * to X-Client-Scan... attributes.
 */
bool SampleBase::preserveAttributes()
{
  VARIANT     firstVariant;
  VARIANT    *attributeValue;
  VARIANT    *attributeName;
  HRESULT     hr;
  SAFEARRAY  *psa;
  long        index[2];
  uint        i;
  _bstr_t     attributeNameBstr;
  std::string attributeNameString;

  /*
   * The trouble with this is that it's really unreasonably
   * _hard_ to do this apparently simple thing -- walk through
   * all the attributes on the sample.  There are several
   * problems -- ranging from the use of OLE VARIANT types
   * to hold attribute values (this is clearly the result of
   * recreation drug use whilst programming) all the way through
   * to the shoddy interfaces.  It's also very hard (impossible)
   * to do it efficiently -- for example, when examining the
   * attribute name, we need to extract it from the VARIANT
   * as a _bstr_t, which we convert to a char *, which we (in
   * turn) convert to a std::string.
   */

  /*
   * Get hold of an interface of the right flavour, and bitch
   * if it doesn't work.
   */
  ID();
  if (!active)
    throw IcePackException(IcePackException::SampleBase,
                           GenErrorMsg(0, "SampleBase::preserveAttributes, unable to activate sample", itemKey.lVal));

  /*
   * Initialise the wacky OLE structure, and then ask the
   * interface for all the attributes on the sample.
   */
  VariantInit(&firstVariant);
  if (FAILED(hr = id->GetAllValues(&firstVariant)))
    throw IcePackException(IcePackException::SampleBase,
                           GenErrorMsg(hr, "SampleBase::preserveAttributes, id->GetAllValues", itemKey.lVal));


  /*
   * Get the array, and bolt it down.
   */
  psa = firstVariant.parray;
  SafeArrayLock(psa);

  /*
   * Ladies and gentlemen, in the event of turbulent code, you
   * will find emergency paper bags in the seat pocket in front
   * of you.
   */
  for (i = 0; i < psa->rgsabound[0].cElements; i++)
  {
    index[0] = i;
    index[1] = 0;
    SafeArrayPtrOfIndex(psa, index, reinterpret_cast<void **>(&attributeName));
    attributeNameBstr   = attributeName->bstrVal;
    attributeNameString = attributeNameBstr;

    /*
     * Skip if the name is invalid.
     */
    if (0 == attributeNameString.length())
      continue;

    /*
     * Blah blah OLE blah blah VARIANT blah blah.
     */
    index[1] = 1;
    SafeArrayPtrOfIndex(psa, index, reinterpret_cast<void **>(&attributeValue));

    /*
     * So, does this attribute exist?
     */
    if (isThisAttributePseudoDeleted(*attributeValue))
      continue;

    /*
     * Is this an X-Scan attribute?
     */
    if (isThisAScanAttribute(attributeNameString))
    {
      std::string clientAttributeNameString(attributeNameString);
      _bstr_t     clientAttributeNameBstr;

      /*
       * Take the attribute copy (which we just made) and pop the
       * 'Client-' section in just after the 'X-'.
       */
      clientAttributeNameString.insert(2, "Client-");

      /*
       * Convert to a _bstr_t (urgh).
       */
      clientAttributeNameBstr = clientAttributeNameString.c_str();

      /*
       * And stamp it in.
       */
      id->SetValue(clientAttributeNameBstr, *attributeValue);
    }

    /*
     * Is this an attribute we not are backing up?  If so,
     * skip on.  One of the tests in here is that the name
     * starts with X-.
     */
    if (turnABlindEyeToThisAttribute(attributeNameString))
      continue;

    /*
     * We need to backup this attribute.
     */

    /*
     * Insert the magic string at the appropriate place in the name.
     */
    attributeNameString.insert(2, "Backup-");

    /*
     * Get a bstring version of the name string (I know, another pointless
     * string copy.  There are just too many ways of representing strings).
     */
    attributeNameBstr = attributeNameString.c_str();

    /*
     * Create the attribute.
     */
    id->SetValue(attributeNameBstr, *attributeValue);
  }

  id->Commit();

  /*
   * Unbolt the array.
   */
  SafeArrayUnlock(psa);
  SafeArrayDestroy(psa);

  /*
   * JD.
   */
  return true;
}

/* ----- */
