#pragma once

// manager macro
#define DECLARE_MGR(class___) public: \
	static class___* get ## class___(); \
	static void set ## class___(class___* mgr_); \
	static void delete ## class___();

#define EXTERN_MGR(class___) extern class___* ___ ## class___;

#define CC_GET_NS(class___) ::get##class___
#define CC_SET_NS(class___) ::set##class___
#define CC_DELETE_NS(class___) ::delete##class___

#define DEFINE_MGR(class___) class___* ___ ## class___ = nullptr; \
	class___* class___ CC_GET_NS(class___) () { return ___## class___; } \
	void class___ CC_SET_NS(class___) (class___* mgr_) { ___ ## class___ = mgr_; } \
	void class___ CC_DELETE_NS(class___) ()  { delete ___ ## class___; } \


// smart ptr
#define DECLARE_SMART_PTR(class___) typedef std::shared_ptr<class___> class___ ## Ptr;

//#define DECLARE_BOOST_SMART_PTR(class___) typedef boost::shared_ptr<class___> class___ ## Ptr;



// tls macro
#define DECLARE_TLS(class___) public: \
	static class___* getTLS ## class___(); \
	static void setTLS ## class___(class___* tls_); \
	static void deleteTLS ## class___();

#define EXTERN_TLS(class___) extern __thread class___* ___TLS ## class___;

#define CC_GETTLS_NS(class___) ::getTLS##class___
#define CC_SETTLS_NS(class___) ::setTLS##class___
#define CC_DELETETLS_NS(class___) ::deleteTLS##class___

#define DEFINE_TLS(class___) __thread class___* ___TLS ## class___ = nullptr; \
	class___* class___ CC_GETTLS_NS(class___) () { return ___TLS ## class___; } \
	void class___ CC_SETTLS_NS(class___) (class___* tls_) { ___TLS ## class___ = tls_; } \
   void class___ CC_DELETETLS_NS(class___) () { delete ___TLS ## class___; }



#define FOWARD_DECLARE(class___) class class___; typedef std::shared_ptr<class___> class___ ## Ptr;