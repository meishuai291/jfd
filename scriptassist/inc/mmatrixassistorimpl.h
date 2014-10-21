/*
 * mmatrixassistorimpl.h
 *
 *  Created on: 2013-10-22
 *      Author: luxuze
 */

#ifndef MMATRIXASSISTORIMPL_H_
#define MMATRIXASSISTORIMPL_H_

#include <qdebug.h>
#include <qvector.h>
#include <mobjectmanager.h>
#include <mextensionmanager.h>
#include <org.sipesc.fems.matrix.mmatrixfactory.h>
#include <org.sipesc.fems.matrix.mmatrixdata.h>
#include <org.sipesc.fems.matrix.melementstiffmatrixdata.h>
#include <org.sipesc.fems.matrix.mmatrix.h>
#include <org.sipesc.fems.matrix.mmatrixtools.h>
#include <org.sipesc.fems.matrixassist.mmatrixassistor.h>

using namespace org::sipesc::fems::matrix;
using namespace org::sipesc::fems::matrixassist;

class MMatrixAssistorImpl: public MMatrixAssistorInterface
{
public:
	MMatrixAssistorImpl()
	{
		MObjectManager objManager = MObjectManager::getManager();
		_mFactory = objManager.getObject(
				"org.sipesc.fems.matrix.matrixfactory");
		Q_ASSERT(!_mFactory.isNull());

		MExtensionManager extManager = MExtensionManager::getManager();
		_mTools = extManager.createExtension(
				"org.sipesc.fems.matrix.MMatrixTools");
		Q_ASSERT(!_mTools.isNull());

		_isSet = false;
	}
	/**
	 * 析构函数. 仅为类继承关系而设置.
	 */
	virtual ~MMatrixAssistorImpl()
	{
	}

public:

	void setMatrixData(const MMatrixData& data)
	{
		if (_isSet)
			_isSet = false;

		MMatrixData mData = data;
		Q_ASSERT(!mData.isNull());
		MMatrix m = _mFactory.createMatrix();
		m << mData;
		_m = m;

		_isSet = true;
	}

	void setStiffMatrixData(const MElementStiffMatrixData& data)
	{
		if (_isSet)
			_isSet = false;

		MElementStiffMatrixData mData = data;
		Q_ASSERT(!mData.isNull());
		MMatrix m = _mFactory.createMatrix();
		m << mData;
		_m = m;

		_isSet = true;
	}

	int getRowCount() const
	{
		Q_ASSERT(_isSet);
		return _m.getRowCount();
	}
	int getColCount() const
	{
		Q_ASSERT(_isSet);
		return _m.getColCount();
	}
	QVector<double> getColumn(int col)
	{
		Q_ASSERT(_isSet);
		return _mTools.getColumn(_m, col);
	}

private:
	MMatrixFactory _mFactory;
	MMatrix _m;
	MMatrixTools _mTools;

	bool _isSet;
};

#endif /* MMATRIXASSISTORIMPL_H_ */
