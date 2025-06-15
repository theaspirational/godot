/**************************************************************************/
/*  factory.cpp                                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include <string>

#include "clips_core/clips.h"
#ifdef STOP
#undef STOP
#endif
#include "core/object/object.h"
#include "core/string/print_string.h"
#include "factory.h"
#include <memory>

Variant data_object_to_variant(void *env, dataObject *clipsdo) {
	return data_object_to_variant(env, *clipsdo);
}

Variant data_object_to_variant(void *env, dataObject &clipsdo) {
	Variant value;
	Array values;
	void *instancePtr;
	dataObject inst_id;
	std::string inst_name;
	std::string inst_id_str;
	std::string delimiter = ":";

	void *mfptr;
	long int end;

	switch (GetType(clipsdo)) {
		case RVOID:
			return value;
		case STRING_CLIPS:
			value = DOToString(clipsdo);
			return value;
		case INSTANCE_NAME:
			inst_name = DOToString(clipsdo);
			inst_id_str = inst_name.substr(inst_name.find(delimiter) + 1);
			if (!std::stoi(inst_id_str)) {
				print_line("clips_es:data_object_to_variant: Error in inst_name");
				print_line(String(inst_name.c_str()));
			}
			value = ObjectDB::get_instance(ObjectID(std::stoi(inst_id_str)));
			return value;
		case SYMBOL:
			if (strcmp(DOToString(clipsdo), "TRUE") == 0) {
				value = true;
			} else if (strcmp(DOToString(clipsdo), "FALSE") == 0) {
				value = false;
			} else {
				value = DOToString(clipsdo);
			}
			return value;
		case FLOAT_CLIPS:
			value = DOToDouble(clipsdo);
			return value;
		case INTEGER:
			value = int64_t(DOToLong(clipsdo));
			return value;
		case INSTANCE_ADDRESS:
			instancePtr = DOToPointer(clipsdo);
			EnvDirectGetSlot(env, instancePtr, "inst_id", &inst_id);
			value = ObjectDB::get_instance(ObjectID(DOToLong(inst_id)));
			return value;
		case EXTERNAL_ADDRESS:
			print_line("clips_es:data_object_to_variant: Detected EXTERNAL_ADDRESS");
			value = DOToExternalAddress(clipsdo);
			return value;

		case MULTIFIELD:
			end = GetDOEnd(clipsdo);
			mfptr = GetValue(clipsdo);
			for (int iter = GetDOBegin(clipsdo); iter <= end; iter++) {
				switch (GetMFType(mfptr, iter)) {
					case STRING_CLIPS:
						value = ValueToString(GetMFValue(mfptr, iter));
						values.push_back(Variant(value));
						break;
					case SYMBOL:
						if ((iter == 1) && ((strcmp(ValueToString(GetMFValue(mfptr, iter)), "Vector2") == 0))) {
							float x = 0;
							float y = 0;

							if (GetMFType(mfptr, iter + 1) == FLOAT_CLIPS) {
								x = ValueToDouble(GetMFValue(mfptr, iter + 1));
							} else if (GetMFType(mfptr, iter + 1) == INTEGER) {
								x = (float)int64_t(ValueToLong(GetMFValue(mfptr, iter + 1)));
							} else {
								print_line("clips_es:data_object_to_variant: Vector2 `x` must be a number");
								// throw std::runtime_error("clips_es:data_object_to_variant: Vector2 `x` must be a number");
							}

							if (GetMFType(mfptr, iter + 2) == FLOAT_CLIPS) {
								y = ValueToDouble(GetMFValue(mfptr, iter + 2));
							} else if (GetMFType(mfptr, iter + 2) == INTEGER) {
								y = (float)int64_t(ValueToLong(GetMFValue(mfptr, iter + 2)));
							} else {
								print_line("clips_es:data_object_to_variant: Vector2 `y` must be a number");
								// throw std::runtime_error("clips_es:data_object_to_variant: Vector2 `y` must be a number");
							}

							value = Vector2(x, y);
							return value;
						} else {
							value = ValueToString(GetMFValue(mfptr, iter));
							values.push_back(Variant(value));
							break;
						}
					case FLOAT_CLIPS:
						value = ValueToDouble(GetMFValue(mfptr, iter));
						values.push_back(Variant(value));
						break;
					case INTEGER:
						value = int64_t(ValueToLong(GetMFValue(mfptr, iter)));
						values.push_back(Variant(value));
						break;
					case INSTANCE_NAME:
						inst_name = ValueToString(GetMFValue(mfptr, iter));
						inst_id_str = inst_name.substr(inst_name.find(delimiter) + 1);
						value = ObjectDB::get_instance(ObjectID(std::stoi(inst_id_str)));
						values.push_back(Variant(value));
						break;
					case INSTANCE_ADDRESS:
						instancePtr = DOToPointer(clipsdo);
						EnvDirectGetSlot(env, instancePtr, "inst_id", &inst_id);
						value = ObjectDB::get_instance(ObjectID(DOToLong(inst_id)));
						values.push_back(Variant(value));
						break;
					case EXTERNAL_ADDRESS:
						value = ValueToExternalAddress(GetMFValue(mfptr, iter));
						values.push_back(Variant(value));
						break;
					default:
						print_line("clips_es:data_object_to_variant: Unhandled multifield type");
				}
			}
			value = values;
			return value;
		default:
			print_line("clips_es::data_object_to_variant: Unhandled data object type");
	}
	value = FALSE;
	return value;
}

dataObject *
variant_to_data_object(void *env, const Variant &value, dataObject *clipsdo, bool as_symbols) {
	Array values;
	Vector2 v2;
	void *p, *p2;
	Object *ref;
	String inst_name;

	if (!clipsdo) {
		clipsdo = new dataObject;
	}

	switch (value.get_type()) {
		case Variant::Type::BOOL:
			SetpType(clipsdo, SYMBOL);
			if (value) {
				SetpValue(clipsdo, EnvTrueSymbol(env));
			} else {
				SetpValue(clipsdo, EnvFalseSymbol(env));
			}
			return clipsdo;
		case Variant::Type::NODE_PATH:
		case Variant::Type::STRING:
			if (as_symbols) {
				SetpType(clipsdo, SYMBOL);
			} else {
				SetpType(clipsdo, STRING_CLIPS);
			}
			p = EnvAddSymbol(env, String(value).ascii().get_data());
			SetpValue(clipsdo, p);
			return clipsdo;
		case Variant::Type::INT:
			SetpType(clipsdo, INTEGER);
			p = EnvAddLong(env, int(value));
			SetpValue(clipsdo, p);
			return clipsdo;
		case Variant::Type::REAL:
			SetpType(clipsdo, FLOAT_CLIPS);
			p = EnvAddDouble(env, float(value));
			SetpValue(clipsdo, p);
			return clipsdo;
		case Variant::Type::OBJECT:
			SetpType(clipsdo, INSTANCE_NAME);
			ref = value;
			inst_name = ref->call("to_string");
			inst_name = inst_name.lstrip("[").rstrip("]");
			p = EnvAddSymbol(env, inst_name.ascii().get_data());
			SetpValue(clipsdo, p);
			return clipsdo;
		case Variant::Type::NIL:
			SetpType(clipsdo, SYMBOL);
			p = EnvAddSymbol(env, "NULL");
			SetpValue(clipsdo, p);
			return clipsdo;
		case Variant::Type::VECTOR2:
			v2 = value;
			p = EnvCreateMultifield(env, 3);

			SetMFType(p, 1, SYMBOL);
			p2 = EnvAddSymbol(env, "Vector2");
			SetMFValue(p, 1, p2);

			SetMFType(p, 2, FLOAT_CLIPS);
			p2 = EnvAddDouble(env, v2.x);
			SetMFValue(p, 2, p2);

			SetMFType(p, 3, FLOAT_CLIPS);
			p2 = EnvAddDouble(env, v2.y);
			SetMFValue(p, 4, p2);

			SetpType(clipsdo, MULTIFIELD);
			SetpValue(clipsdo, p);
			SetpDOBegin(clipsdo, 1);
			SetpDOEnd(clipsdo, 3);
			return clipsdo;
		case Variant::Type::POOL_BYTE_ARRAY:
		case Variant::Type::POOL_INT_ARRAY:
		case Variant::Type::POOL_REAL_ARRAY:
		case Variant::Type::POOL_STRING_ARRAY:
		case Variant::Type::ARRAY:
			values = value;
			p = EnvCreateMultifield(env, values.size());
			for (unsigned int iter = 0; iter < values.size(); iter++) {
				unsigned int mfi = iter + 1; // mfptr indices start at 1

				switch (values[iter].get_type()) {
					case Variant::Type::BOOL:
						SetMFType(p, mfi, SYMBOL);
						if (values[iter]) {
							p2 = EnvAddSymbol(env, "TRUE");
						} else {
							p2 = EnvAddSymbol(env, "FALSE");
						}
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::NODE_PATH:
					case Variant::Type::STRING:
						if (as_symbols) {
							SetMFType(p, mfi, SYMBOL);
						} else {
							SetMFType(p, mfi, STRING_CLIPS);
						}
						p2 = EnvAddSymbol(env, String(values[iter]).ascii().get_data());
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::INT:
						SetMFType(p, mfi, INTEGER);
						p2 = EnvAddLong(env, int(values[iter]));
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::REAL:
						SetMFType(p, mfi, FLOAT_CLIPS);
						p2 = EnvAddDouble(env, float(values[iter]));
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::NIL:
						SetMFType(p, mfi, SYMBOL);
						p2 = EnvAddSymbol(env, "NULL");
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::OBJECT:
						SetMFType(p, mfi, INSTANCE_NAME);
						ref = values[iter];
						inst_name = ref->call("to_string");
						inst_name = inst_name.lstrip("[").rstrip("]");
						p2 = EnvAddSymbol(env, inst_name.ascii().get_data());
						SetMFValue(p, mfi, p2);
						break;
					case Variant::Type::ARRAY:
						SetMFType(p, mfi, SYMBOL);
						p2 = EnvAddSymbol(env, "[NESTED_ARRAY]");
						SetMFValue(p, mfi, p2);
						break;
						break;
					default:
						print_line("clips_es::variant_to_data_object ARRAY: Unhandled data object type: " + values[iter].get_type_name(values[iter].get_type()));
				}
			}
			SetpType(clipsdo, MULTIFIELD);
			SetpValue(clipsdo, p);
			SetpDOBegin(clipsdo, 1);
			SetpDOEnd(clipsdo, values.size());
			return clipsdo;
		default:
			print_line("clips_es::variant_to_data_object: Unhandled data object type: " + value.get_type_name(value.get_type()));
	}

	SetpType(clipsdo, SYMBOL);
	SetpValue(clipsdo, EnvFalseSymbol(env));
	return clipsdo;
}
