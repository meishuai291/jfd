/*
 * format.h
 *
 *  Created on: 2014年10月27日
 *      Author: sipesc
 */

#ifndef FORMAT_H_
#define FORMAT_H_

#include <QString>

namespace org {
namespace sipesc {
namespace fems {
namespace bltexport {

class BltForamt{
public:
	BltForamt(){}
	~BltForamt(){}

public:
	// 补空格, val为整数,w总为占位数
	static QString blank(int val, int w){

		QString string = QString::number(val, 10);

		int n = w - string.length();
		for (int t = 0; t < n; t++)
			string = QString(" ") + string;
		return string;
	}

	//科学计数法，i为保留有效数字,w总为占位数
	static QString sciNot(double val, int i, int w){
		QString string = QString("%1").arg(val, w, 'E', i - 1);
		return string;
	}
};

}}}}



#endif /* FORMAT_H_ */
