// lckobj.h - Lockable Object header
// Pure virtual base class

#if !defined __LCKOBJ_H__
#define __LCKOBJ_H__

class LockableObject
{
public:
  LockableObject() {};
  virtual ~LockableObject() {};
  virtual void capture() = 0;
  virtual void release() = 0;
};
#endif // __LCKOBJ_H__
