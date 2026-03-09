#pragma once

#include <vector>
#include <string>

namespace oldking
{
	class my_exception
	{
	public:
		my_exception(const std::string& errmsg = "", const int& errid = -1)
			:_errmsg(errmsg)
			, _errid(errid)
		{}
	
		virtual std::string what() const
		{
			return _errmsg;
		}
	
		int get_id() const
		{
			return _errid;
		}
	
	protected:
		std::string _errmsg; //异常文字信息
		int _errid; //异常id
	};
	
	class func1_exception : public my_exception
	{
	public:
		func1_exception(const std::string& errormsg = "", const int& errid = -1)
			: my_exception(errormsg, errid)
		{}
	
		std::string what() const override
		{
			std::string ret;
			ret += my_exception::_errmsg;
			ret += ":";
			ret += std::to_string(my_exception::_errid);
	
			return ret;
		}
	
	private:
	};
}
