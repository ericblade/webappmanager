// Copyright (c) 2014-2021 LG Electronics, Inc.
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

#include "json/json.h"

#include "ApplicationDescription.h"
#include "LogManager.h"
#include "PalmSystemBlink.h"
#include "Utils.h"
#include "WebAppBase.h"
#include "WebAppWayland.h"
#include "WebPageBlink.h"

namespace {

const char* toStr(const bool value)
{
    return value ? "true" : "false";
}

std::string toStr(const Json::Value& object)
{
    static Json::StreamWriterBuilder builder;
    return Json::writeString(builder, object);
}

} // namespace

PalmSystemBlink::PalmSystemBlink(WebAppBase* app)
    : PalmSystemWebOS(app)
    , m_initialized(false)
{
}

std::string PalmSystemBlink::handleBrowserControlMessage(const std::string& command, const std::vector<std::string>& arguments)
{
    if (command == "initialize") {
        return toStr(initialize());
    } else if (command == "country") {
        return country().toStdString();
    } else if (command == "locale") {
        return locale().toStdString();
    } else if (command == "localeRegion") {
        return localeRegion().toStdString();
    } else if (command == "isMinimal") {
        return toStr(isMinimal());
    } else if (command == "identifier") {
        return identifier().toStdString();
    } else if (command == "screenOrientation") {
        return screenOrientation().toStdString();
    } else if (command == "currentCountryGroup") {
        return getDeviceInfo("CountryGroup").toStdString();
    } else if (command == "stageReady") {
        stageReady();
    } else if (command == "activate") {
        LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.activate()");
        activate();
    } else if (command == "deactivate") {
        LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.deactivate()");
        deactivate();
    } else if (command == "isActivated") {
        return toStr(isActivated());
    } else if (command == "isKeyboardVisible") {
        return toStr(isKeyboardVisible());
    } else if (command == "getIdentifier" || command == "identifier") {
        return QString(identifier().toUtf8()).toStdString();
    } else if (command == "launchParams") {
        LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.launchParams Updated by app; %s", arguments[0].c_str());
        updateLaunchParams(QString::fromStdString(arguments[0]));
    } else if (command == "screenOrientation") {
        return screenOrientation().toStdString();
    } else if (command == "keepAlive") {
        if (arguments.size() > 0)
            setKeepAlive(arguments[0] == "true");
    } else if (command == "PmLogInfoWithClock") {
        if (arguments.size() == 3)
            pmLogInfoWithClock(QString::fromStdString(arguments[0]), QString::fromStdString(arguments[1]), QString::fromStdString(arguments[2]));
    } else if (command == "PmLogString") {
        if (arguments.size() > 3) {
            int32_t v1;
            if (strToInt(arguments[0], v1))
                pmLogString(static_cast<PmLogLevel>(v1), QString::fromStdString(arguments[1]), QString::fromStdString(arguments[2]), QString::fromStdString(arguments[3]));
        }
    } else if (command == "setWindowProperty") {
        if (arguments.size() > 1) {
            LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()),
                "webOSSystem.window.setProperty('%s', '%s')", arguments[0].c_str(), arguments[1].c_str());
            m_app->setWindowProperty(QString::fromStdString(arguments[0]), QString::fromStdString(arguments[1]));
        }
    } else if (command == "platformBack") {
        LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.platformBack()");
        m_app->platformBack();
    } else if (command == "setCursor") {
        if (arguments.size() == 3) {
            std::string v1 = arguments[0];
            int32_t v2, v3;
            const bool v2_conversion = strToInt(arguments[1], v2);
            const bool v3_conversion = strToInt(arguments[2], v3);
            if (v2_conversion && v3_conversion)
                m_app->setCursor(QString::fromStdString(v1), v2, v3);
        }
    } else if (command == "setInputRegion") {
        QByteArray data;
        for (const auto& argument : arguments) {
            data.append(QString::fromStdString(argument).toUtf8());
        }
        setInputRegion(data);
    } else if (command == "setKeyMask") {
        QByteArray data;
        for (const auto& argument : arguments) {
            data.append(QString::fromStdString(argument).toUtf8());
        }
        setGroupClientEnvironment(KeyMask, data);
    } else if (command == "focusOwner") {
        setGroupClientEnvironment(FocusOwner, NULL);
    } else if (command == "focusLayer") {
        setGroupClientEnvironment(FocusLayer, NULL);
    } else if (command == "hide") {
        hide();
    } else if (command == "setLoadErrorPolicy") {
        if (arguments.size() > 0) {
            LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.setLoadErrorPolicy(%s)", arguments[0].c_str());
            setLoadErrorPolicy(QString::fromStdString(arguments[0]));
        }
    } else if (command == "onCloseNotify") {
        if (arguments.size() > 0) {
            LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.onCloseNotify(%s)", arguments[0].c_str());
            onCloseNotify(QString::fromStdString(arguments[0]));
        }
    } else if (command == "cursorVisibility") {
        return toStr(cursorVisibility());
    } else if (command == "serviceCall") {
        if (m_app->page()->isClosing()) {
          LOG_INFO(MSGID_PALMSYSTEM, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())), PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())), PMLOGKFV("PID", "%d", m_app->page()->getWebProcessPID()), "webOSSystem.serviceCall(%s, %s)", arguments[0].c_str(), arguments[1].c_str());
          m_app->serviceCall(QString::fromStdString(arguments[0]), QString::fromStdString(arguments[1]), m_app->appId());
        } else {
            LOG_WARNING(MSGID_SERVICE_CALL_FAIL, 3, PMLOGKS("APP_ID", qPrintable(m_app->appId())),
              PMLOGKS("INSTANCE_ID", qPrintable(m_app->instanceId())),
              PMLOGKS("URL", arguments[0].c_str()), "Page is NOT in closing");
        }
    }

    return std::string();
}

void PalmSystemBlink::setCountry()
{
    static_cast<WebPageBlink*>(m_app->page())->updateExtensionData(QStringLiteral("country"), country());
}

void PalmSystemBlink::setLaunchParams(const QString& params)
{
    PalmSystemWebOS::setLaunchParams(params);
    static_cast<WebPageBlink*>(m_app->page())->updateExtensionData(QStringLiteral("launchParams"), launchParams());
}

void PalmSystemBlink::setLocale(const QString& params)
{
    static_cast<WebPageBlink*>(m_app->page())->updateExtensionData(QStringLiteral("locale"), params);
}

QString PalmSystemBlink::identifier() const
{
    if (!m_app->page())
        return QStringLiteral("");

    return static_cast<WebPageBlink*>(m_app->page())->getIdentifier();
}

void PalmSystemBlink::setLoadErrorPolicy(const QString& params)
{
    static_cast<WebPageBlink*>(m_app->page())->setLoadErrorPolicy(params);
}

std::string PalmSystemBlink::trustLevel() const
{
    return static_cast<WebPageBlink*>(m_app->page())->trustLevel().toStdString();
}

void PalmSystemBlink::onCloseNotify(const QString& params)
{
    if (params == "didSetOnCloseCallback")
        static_cast<WebPageBlink*>(m_app->page())->setHasOnCloseCallback(true);
    else if (params == "didClearOnCloseCallback")
        static_cast<WebPageBlink*>(m_app->page())->setHasOnCloseCallback(false);
    else if (params == "didRunOnCloseCallback")
        static_cast<WebPageBlink*>(m_app->page())->didRunCloseCallback();
}

double PalmSystemBlink::devicePixelRatio()
{
    return static_cast<WebPageBlink*>(m_app->page())->devicePixelRatio();
}

Json::Value PalmSystemBlink::initialize()
{
    m_initialized = true;

    Json::Value data;
    data["launchParams"] = launchParams().toStdString();
    data["country"] = country().toStdString();
    data["tvSystemName"] = getDeviceInfo("TvSystemName").toStdString();
    data["currentCountryGroup"] = getDeviceInfo("CountryGroup").toStdString();
    data["locale"] = locale().toStdString();
    data["localeRegion"] = localeRegion().toStdString();
    data["isMinimal"] = isMinimal();
    data["identifier"] = identifier().toStdString();
    data["screenOrientation"] = screenOrientation().toStdString();
    data["deviceInfo"] = getDeviceInfo("TvDeviceInfo").toStdString();
    data["activityId"] = (double)activityId();
    data["phoneRegion"] = phoneRegion().toStdString();
    data["folderPath"] = m_app->getAppDescription()->folderPath();

    data["devicePixelRatio"] = devicePixelRatio();
    data["trustLevel"] = trustLevel();
    return data;
}


