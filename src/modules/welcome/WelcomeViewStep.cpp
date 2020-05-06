/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   Copyright 2014-2015, Teo Mrnjavac <teo@kde.org>
 *   Copyright 2018, Adriaan de Groot <groot@kde.org>
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WelcomeViewStep.h"

#include "Config.h"
#include "WelcomePage.h"
#include "checker/GeneralRequirements.h"

#include "Branding.h"
#include "geoip/Handler.h"
#include "locale/Lookup.h"
#include "modulesystem/ModuleManager.h"
#include "utils/Logger.h"
#include "utils/Variant.h"

#include <QFutureWatcher>
#include <QVariant>

CALAMARES_PLUGIN_FACTORY_DEFINITION( WelcomeViewStepFactory, registerPlugin< WelcomeViewStep >(); )

WelcomeViewStep::WelcomeViewStep( QObject* parent )
    : Calamares::ViewStep( parent )
    , m_requirementsChecker( new GeneralRequirements( this ) )
    , m_conf( new Config( this ) )
{
    connect( Calamares::ModuleManager::instance(),
             &Calamares::ModuleManager::requirementsComplete,
             this,
             &WelcomeViewStep::nextStatusChanged );

    // the instance of the qqc2 or qwidgets page
    m_widget = new WelcomePage( m_conf );
    connect( m_conf, &Config::localeIndexChanged, m_widget, &WelcomePage::externallySelectedLanguage );
}

WelcomeViewStep::~WelcomeViewStep()
{
    if ( m_widget && m_widget->parent() == nullptr )
    {
        m_widget->deleteLater();
    }
}

QString
WelcomeViewStep::prettyName() const
{
    return tr( "Welcome" );
}


QWidget*
WelcomeViewStep::widget()
{
    return m_widget;
}


bool
WelcomeViewStep::isNextEnabled() const
{
    return m_widget->verdict();
}


bool
WelcomeViewStep::isBackEnabled() const
{
    return false;
}


bool
WelcomeViewStep::isAtBeginning() const
{
    return true;
}


bool
WelcomeViewStep::isAtEnd() const
{
    return true;
}


Calamares::JobList
WelcomeViewStep::jobs() const
{
    return Calamares::JobList();
}


void
WelcomeViewStep::setConfigurationMap( const QVariantMap& configurationMap )
{
    m_conf->setConfigurationMap( configurationMap );

    if ( configurationMap.contains( "requirements" )
         && configurationMap.value( "requirements" ).type() == QVariant::Map )
    {
        m_requirementsChecker->setConfigurationMap( configurationMap.value( "requirements" ).toMap() );

        m_conf->requirementsModel().setRequirementsList( checkRequirements() );
    }
    else
        cWarning() << "no valid requirements map found in welcome "
                      "module configuration.";

    bool ok = false;
    QVariantMap geoip = CalamaresUtils::getSubMap( configurationMap, "geoip", ok );
    if ( ok )
    {
        using FWString = QFutureWatcher< QString >;

        auto* handler = new CalamaresUtils::GeoIP::Handler( CalamaresUtils::getString( geoip, "style" ),
                                                            CalamaresUtils::getString( geoip, "url" ),
                                                            CalamaresUtils::getString( geoip, "selector" ) );
        if ( handler->type() != CalamaresUtils::GeoIP::Handler::Type::None )
        {
            auto* future = new FWString();
            connect( future, &FWString::finished, [ view = this, f = future, h = handler ]() {
                QString countryResult = f->future().result();
                cDebug() << "GeoIP result for welcome=" << countryResult;
                view->setCountry( countryResult, h );
                f->deleteLater();
                delete h;
            } );
            future->setFuture( handler->queryRaw() );
        }
        else
        {
            // Would not produce useful country code anyway.
            delete handler;
        }
    }

    //here init the qml or qwidgets needed bits
    m_widget->init();
}

Calamares::RequirementsList
WelcomeViewStep::checkRequirements()
{
    return m_requirementsChecker->checkRequirements();
}

static inline void
logGeoIPHandler( CalamaresUtils::GeoIP::Handler* handler )
{
    if ( handler )
    {
        cDebug() << Logger::SubEntry << "Obtained from" << handler->url() << " ("
                 << static_cast< int >( handler->type() ) << handler->selector() << ')';
    }
}

void
WelcomeViewStep::setCountry( const QString& countryCode, CalamaresUtils::GeoIP::Handler* handler )
{
    if ( countryCode.length() != 2 )
    {
        cDebug() << "Unusable country code" << countryCode;
        logGeoIPHandler( handler );
        return;
    }

    auto c_l = CalamaresUtils::Locale::countryData( countryCode );
    if ( c_l.first == QLocale::Country::AnyCountry )
    {
        cDebug() << "Unusable country code" << countryCode;
        logGeoIPHandler( handler );
        return;
    }
    else
    {
        int r = CalamaresUtils::Locale::availableTranslations()->find( countryCode );
        if ( r < 0 )
        {
            cDebug() << "Unusable country code" << countryCode << "(no suitable translation)";
        }
        if ( ( r >= 0 ) && m_conf )
        {
            m_conf->setCountryCode( countryCode );
        }
    }
}
