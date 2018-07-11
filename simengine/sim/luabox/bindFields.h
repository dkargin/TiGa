#ifndef LUABOX_BIND_FIELDS
#define LUABOX_BIND_FIELDS

namespace LuaBox
{
	template<class Type> inline TypeBindings<Type> * getLuaType(TypeBindings<Type> *newptr);
}

struct Field
{
	typedef lua_State Stream;
	// base structure for any field pointers. Base - owner type.
	template<class Target> class Base
	{
	public:
		virtual ~Base(){};
		virtual bool read(Stream *stream,Target *base)=0;	// reading field from stream
		virtual bool write(Stream *stream,Target *base)=0;// storing field to stream
		virtual bool canRead()
		{
			return true;
		}
		virtual bool canWrite()
		{
			return true;
		}
		void error(const char *msg,const char *from=NULL)
		{
			printf("Error in serialising: %s\n",msg);
		}
	};	

	template<class Target,class Type> class GetSet: public Field::Base<Target>
	{
	protected:
		typedef Type (Target::*FunctionGet)(void);
		typedef void (Target::*FunctionSet)(Type &value);
		FunctionGet get;
		FunctionSet set;
		Type *def;

		virtual void setval(Target *base,Type &val)
		{
			(base->*set)(val);
		}
		virtual Type getval(Target *base)
		{
			return (base->*get)();
		}
	public:
		GetSet(FunctionGet fnGet,FunctionSet fnSet,Type *_default)
			:get(fnGet),set(fnSet),def(_default)
		{}
		bool read(Stream *stream,Target *base)
		{
			Type value;
			if(!set)
			{
				error("invalid set function","ComplexFieldPtr::readNode");
				return false;
			}
			if(!SerialiserBase<Type>::sread(getSerialiser<Type>(NULL),stream,&value,name.c_str()))
			{
				error("stream reading error");
				return false;
			}
			setval(base,value);
		}
		bool canRead()
		{
			return set!=NULL;
		}
		bool canWrite()
		{
			return get!=NULL;
		}
		bool write(Stream *stream,Target *base)
		{	
			if(!get)
			{
				error("invalid get function","ComplexFieldPtr::readNode");
				return false;
			}
			Type value=getval(base);
			LuaBox::LuaType<Type> *serialiser=LuaBox::getLuaType<Type>();
			//return LuaBox::getLuaType<Type>()->write(stream,reference(base));

			if(!SerialiserBase<Type>::swrite(getSerialiser<Type>(NULL),stream,&value,name.c_str()))
			{
				error("stream writing error");
				return false;
			}
			return true;
		}	
	};
	// base structure for typed field pointers. Base - owner type. Type - field type
	template<class Target,class Type> class Typed: public Field::Base<Target>
	{
	protected:
		Type Target::* offset;
		Type *def;				// default value. Is it useless?	
		typedef Type value_type;
	public:
		Typed(Type Target::* _offset,Type *_def)
			:offset(_offset),def(_def)
		{}
		const Type* pointer(const Target *base) const
		{
			return &(base->*offset);
		}
		Type* pointer(Target *base) const
		{
			return &(base->*offset);
		}
		const Type& reference(const Target *base) const
		{
			return base->*offset;
		}
		Type& reference(Target *base) const
		{
			return base->*offset;
		}
	};
	// pointer to a container field type. Base - field owner. Type - container field type (i.e std::list<int>)
	// does not support complex values (i.e std::list<Point)
	template<class Target,class Type> class Container: public Typed<Target,Type>
	{
	public:
		typedef Type ContainerType;
		Container(const char *name,Type Target::* offset,Type *def)
			:Typed<Target,Type>(name,offset,def)
		{
			printf("Container field constructor\n");
		}
		virtual ~Container()
		{
			printf("Container field destructor\n");
		}	
		bool read(Stream *stream,Target *base)
		{
			printf("Container field reading %s\n",name.c_str());
			ContainerType &container=reference(base);
			int size=0;
			container.clear();
			stream->read(size,(name+"-size").c_str());
			printf("- container size = %d\n",size);
			while(size--)
			{
				ContainerType::value_type value;
				stream->read(value,name.c_str());
				addToContainer(container,value);
			}
			return true;
		}
		bool write(Stream *stream,Target *base)
		{
			printf("Container field writing %s\n",name.c_str());
			ContainerType &container=reference(base);
		//	int o=(char*)&container-(char*)base;
			
			ContainerType::iterator it=container.begin();
			int size=(int)container.size();
			stream->write(size,(name+"-size").c_str());
			for(;it!=container.end();it++)
				stream->write((*it),name.c_str());		
			return true;
		}
	protected:
		// generic insertion for vector-based containers. 
		template<class T> static void addToContainer(std::vector<T> &container,const T &value)
		{
			container.push_back(value);
		}
		// generic insertion for list-based containers.
		template<class T> static void addToContainer(std::list<T> &container,const T &value)
		{
			container.push_back(value);
		}
		// generic insertion for tree-based containers. For map or multimap - insert a pair
		template<class Traits> static void addToContainer(std::_Tree<Traits> &container,const typename std::_Tree<Traits>::value_type &value)
		{
			container.insert(container.begin(),value);
		}	
	};
	// simple
	template<class Target,class Type> class Simple: public Typed<Target,Type>
	{
	public:
		Simple(Type Target::* offset,Type *def)
			:Typed<Target,Type>(offset,def)
		{}
		virtual ~Simple()
		{
			printf("Simple field destructor\n");
		}	
		bool write(Stream *stream,Target *base)
		{	
			//LuaBox::TypeIO<Type> *serialiser=LuaBox::getLuaType<Type>();
			return LuaBox::TypeBindings<Type>::write(stream,reference(base));
		}
		bool read(Stream *stream,Target *base)
		{	
			reference(base)=LuaBox::TypeBindings<Type>::get(stream,-1);
			return true;
		}
	};
	// pointer to complex field type. Base - field owner. Type - field type
	template<class Target,class Type> class Complex: public Typed<Target,Type>
	{
	public:
		Complex(Type Target::* offset,Type *def)
			:Typed<Target,Type>(offset,def)
		{}
		virtual ~Complex()
		{
			printf("Complex field destructor\n");
		}	
		bool write(Stream *stream,Target *base)
		{	
			LuaBox::TypeBindings<Type&> *serialiser=LuaBox::getLuaType<Type&>();
			return serialiser->write(stream,reference(base));
		}
		bool read(Stream *stream,Target *base)
		{	
			reference(base)=LuaBox::getLuaType<Type&>()->get(stream,-1);
			return true;
		}
	};
};
#endif