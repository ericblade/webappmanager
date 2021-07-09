// Copyright (c) 2021 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <string>

// TODO: remove header when QT less implementation will be completed.
#include <QJsonObject>

namespace Json {
class Value;
};

namespace util {

bool JsonValueFromString(const std::string& str, Json::Value& value);
std::string StringFromJsonValue(const Json::Value& value);

// TODO: remove this methods when QT less implementation will be completed.
QJsonObject QJsonObjectFromJsonValue(const Json::Value& value);
bool JsonValueFromQJsonObject(const QJsonObject& object, Json::Value& value);

}  // namespace util

#endif // JSON_HELPER_H
