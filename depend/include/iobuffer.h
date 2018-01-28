#pragma once

#include <istream>
#include <ostream>
#include <exception>

namespace frosttools
{

/// Contains several helpers for safe IO operations.
/// Seems to be deprecated

/**
 * exception for buffer reading process. is throwed in xread/xwrite functions
 */

struct XRead
{
	int read;	///< bytes read
	int asked;	///< bytes asked
	/// Constructor
	XRead(int r,int a):read(r),asked(a) {}
};

/**
 * reads <size> bytes from stream.
 * Also check the number of bytes read with the number of bytes asked, throwing exception
 */
///////////////////////////////////////////////////////////////////////////////////
// streambuf io
///////////////////////////////////////////////////////////////////////////////////
/// read data from stream
inline bool xread(std::streambuf &stream,void *data,int size)
{
	int count=stream.sgetn((char*)data,size);
	if(count!=size)
		throw(XRead(count,size));
	return count==size;
}
/// write data to stream
inline bool xwrite(std::streambuf &stream,const void *data,int size)
{
	stream.sputn((const char*)data,size);
	return true;
}
/// check if end of file has occured
inline bool xeof(std::streambuf &stream)
{
	int s=stream.in_avail();
	return s==0;
}
////////////////////////////////////////////////////////////////////////////////////
// stream io
////////////////////////////////////////////////////////////////////////////////////
inline bool xread(std::istream &stream,void *data,int size)
{
	int count=stream.readsome((char*)data,size);
	//int count=stream.sgetn((char*)data,size);
	if(count!=size)
		throw(XRead(count,size));
	return count==size;
}
inline bool xwrite(std::ostream &stream,const void *data,int size)
{
	stream.write((const char*)data,size);
	return true;
}
inline bool xeof(std::ios &stream)
{
	return stream.eof();
}

/// Buffer IO
class IOBuffer
{	
	char * buffer;	///< allocated buffer data
	int bufferSize;	///< allocated buffer size
	typedef char * BufferPtr;	///< buffer pointer type
	BufferPtr begin,end;	///< cover actual data.
	BufferPtr current;		///< locked inside (begin,end)

	bool bRead,bLocked;
public:
	enum
	{
		minSize=256
	};

	/// Current buffer position
	int position;

	/// Constructor
	IOBuffer(bool in,int max=minSize)
		:bRead(in),bLocked(false),current(0),begin(0),buffer(NULL),bufferSize(0)
	{
		if(!bRead)resize(max);
	}
	/// place current position to start
	void rewind()
	{
		current=begin;
	}
	/// get current size
	int size()const
	{
		return current-begin;
	}
	/// get size left
	int left()const
	{
		return end-current;
	}
	/// write data to the buffer
	int write(const void *data,int size)
	{
		if(!out())
			_xwrite();
		if(current+size>buffer+bufferSize)
			resize(bufferSize+size);

		memcpy(current,data,size);

		/*
		if(!_CrtCheckMemory( ))
			_xmem();*/

		current+=size;
		return size;
	}
	/// write data to the buffer
	int write(const IOBuffer &buffer)
	{
		return write(buffer.begin,buffer.size());
	}

	/// check if we reached end of buffer
	bool eof()
	{
		return end==current;
	}	
	/// read data from the buffer
	int read(void *data,int size)
	{
		if(!in())_xread();
		if(current+size<=end)
		{
			memcpy(data,current,size);
			current+=size;
			return size;
		}
		else
			throw(std::runtime_error("IOBuffer eof"));
	}

	/// get data before current position (some sort of 'putback')
	template<class Type> Type& getBefore(int position)
	{
		if(begin+position>end)
			throw(std::exception("IOBuffer eof"));
		return *reinterpret_cast<Type*>(begin+position-sizeof(Type));
	}

	/// get data after current position
	template<class Type> Type& getAfter(int position)
	{
		if(begin+position+sizeof(Type)>end)
			throw(std::exception("IOBuffer eof"));
		return *reinterpret_cast<Type*>(begin+position);
	}
	/// get current position
	size_t pos() const
	{
		return (size_t)(current-begin);
	}
	/// set current position
	void setPos(size_t p)
	{
		if(p>=0 && p<=end-begin)
			current=begin+p;
		else
			_xlen(p,0);

	}
	/// write generic to the buffer
	template<class Type> int write(const Type &t)	// returns position
	{
		write(&t,sizeof(Type));
		return current-sizeof(Type)-begin;
	}
	/// read generic from the buffer
	template<class Type> int read(Type &t)
	{
		int count=read(&t,sizeof(Type));
		if(count!=sizeof(Type))
			throw(XRead(count,sizeof(Type)));
		return count;
	}
	/// Switch modes
	bool flip()
	{
		if(!bLocked)
		{
			if(bRead)
				end=begin+bufferSize;
			else
				end=current;
			current=begin;
			bRead=!bRead;
		}
		return bRead;
	}
	/// check if buffer works in 'Read' mode
	bool in()const
	{
		return bRead;
	}
	/// check if buffer works in 'Write' mode
	bool out()const
	{
		return !bRead;
	}
	
	/// read data from the buffer
	IOBuffer readBuffer(int length)
	{
		if(current>=end || current+length>end)_xlen(current-begin,length);
		if(!in())_xread();	// only for read only buffers
		IOBuffer result(*this,current-begin,length);
		current+=length;
		return result;
	}

	~IOBuffer()
	{
		if(buffer && !bLocked)
		{
			free(buffer);
			buffer=NULL;
		}
	}
protected:
	/// construct buffer pointing to region of other buffer
	IOBuffer(IOBuffer &buffer, int start, int length)
	{
		position = 0;
		bRead = true;
		bLocked = true;
		this->buffer = nullptr;
		bufferSize = 0;
		begin=buffer.begin+start;
		current=begin;
		end=begin+length;
	}
	/// calculate appropriate size
	static int calcSize(int newsize)
	{
		int result=1;
		while(result<newsize)
			result*=2;
		return result;
	}
	/// resize data buffer, and init pointers
	void resize(int newsize=minSize)
	{
		newsize=calcSize(newsize);
		if(in())_xread();
		if(bLocked)_xlocked();
		int offs=current-begin;		
		if((buffer=reinterpret_cast<char *>(buffer?realloc(buffer,newsize):malloc(newsize)))==NULL)_xmem();	// throw exception
		bufferSize=newsize;
		
		begin=reinterpret_cast<char *>(buffer);
		end=begin+bufferSize;
		current=begin+offs;
	}
	/// throw memory exception
	void _xmem()
	{
		throw(std::runtime_error("memory error"));
	}
	/// throw 'locked' exception
	void _xlocked()
	{
		throw(std::runtime_error("buffer is locked"));
	}
	/// throw 'len' exception
	void _xlen(int pos,int len)
	{
		throw(std::runtime_error("buffer overflow"));
	}
	/// throw 'read only' exception
	void _xread()
	{
		throw(std::runtime_error("read only"));
	}
	/// throw 'write only' exception
	void _xwrite()
	{
		throw(std::runtime_error("write only"));
	}
};

typedef IOBuffer MsgBuffer;
typedef IOBuffer MsgBufferImpl;

} // namespace frosttools
