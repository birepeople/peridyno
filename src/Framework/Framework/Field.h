#pragma once
#include <iostream>
#include "FieldBase.h"
#include "Array/Array.h"
#include "Array/ArrayList.h"

namespace dyno {
	/*!
	*	\class	Variable
	*	\brief	Variables of build-in data types.
	*/
	template<typename T>
	class VarField : public FieldBase
	{
	public:
		typedef T				VarType;
		typedef T				DataType;
		typedef VarField<T>		FieldType;

		DEFINE_FIELD_FUNC(FieldType, DataType, VarField);

		VarField(T value, std::string name, std::string description, FieldTypeEnum fieldType, Base* parent);
		~VarField() override;

		uint getElementCount() override { return 1; }

		void setValue(T val);
	};

	template<typename T>
	VarField<T>::VarField(T value, std::string name, std::string description, FieldTypeEnum fieldType, Base* parent)
		: FieldBase(name, description, fieldType, parent)
	{
		this->setValue(value);
	}

	template<typename T>
	VarField<T>::~VarField()
	{
	};

	template<typename T>
	void VarField<T>::setValue(T val)
	{
		std::shared_ptr<T>& data = this->getDataPtr();
		if (data == nullptr)
		{
			data = std::make_shared<T>(val);
		}
		else
		{
			*data = val;
		}

		//this->update();
	}

	template<typename T>
	using HostVarField = VarField<T>;

	template<typename T>
	using DeviceVarField = VarField<T>;


	/**
	 * Define field for Array
	 */
	template<typename T, DeviceType deviceType>
	class ArrayField : public FieldBase
	{
	public:
		typedef T							VarType;
		typedef Array<T, deviceType>		DataType;
		typedef ArrayField<T, deviceType>	FieldType;

		DEFINE_FIELD_FUNC(FieldType, DataType, ArrayField);

		~ArrayField() override;

		inline uint getElementCount() override {
			auto ref = this->getDataPtr();
			return ref == nullptr ? 0 : ref->size();
		}

		void setElementCount(uint num);

		void setValue(std::vector<T>& vals);
		void setValue(DArray<T>& vals);
	};

	template<typename T, DeviceType deviceType>
	ArrayField<T, deviceType>::~ArrayField()
	{
		if (m_data.use_count() == 1)
		{
			m_data->clear();
		}
	}

	template<typename T, DeviceType deviceType>
	void ArrayField<T, deviceType>::setElementCount(uint num)
	{
		FieldBase* topField = this->getTopField();
		ArrayField<T, deviceType>* derived = dynamic_cast<ArrayField<T, deviceType>*>(topField);

		if (derived->m_data == nullptr)
		{
			derived->m_data = std::make_shared<Array<T, deviceType>>(num);
		}
		else
		{
			derived->m_data->resize(num);
		}
	}

	template<typename T, DeviceType deviceType>
	void ArrayField<T, deviceType>::setValue(std::vector<T>& vals)
	{
		std::shared_ptr<Array<T, deviceType>>& data = this->getDataPtr();
		if (data == nullptr)
		{
			data = std::make_shared<Array<T, deviceType>>();
		}

		data->assign(vals);
	}

	template<typename T, DeviceType deviceType>
	void ArrayField<T, deviceType>::setValue(DArray<T>& vals)
	{
		std::shared_ptr<Array<T, deviceType>>& data = this->getDataPtr();
		if (data == nullptr)
		{
			data = std::make_shared<Array<T, deviceType>>();
		}

		data->assign(vals);
	}

	template<typename T>
	using HostArrayField = ArrayField<T, DeviceType::CPU>;

	template<typename T>
	using DeviceArrayField = ArrayField<T, DeviceType::GPU>;


	/**
	 * Define field for Array
	 */
	template<typename T, DeviceType deviceType>
	class ArrayListField : public FieldBase
	{
	public:
		typedef T								VarType;
		typedef ArrayList<T, deviceType>		DataType;
		typedef ArrayListField<T, deviceType>	FieldType;

		DEFINE_FIELD_FUNC(FieldType, DataType, ArrayListField);

		inline uint getElementCount() override {
			auto ref = this->getDataPtr();
			return ref == nullptr ? 0 : ref->size();
		}
	};
}