/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef __FRAMEWORK_UIFACTORY_FACTORYCONFIGURATION_HXX_
#define __FRAMEWORK_UIFACTORY_FACTORYCONFIGURATION_HXX_
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XUIControllerRegistration.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase1.hxx>
#include <rtl/ustring.hxx>

//_________________________________________________________________________________________________________________
//	Namespace
//_________________________________________________________________________________________________________________
// 

namespace framework
{

//*****************************************************************************************************************
//	Configuration access class for PopupMenuControllerFactory implementation
//*****************************************************************************************************************
class ConfigurationAccess_ControllerFactory : // interfaces
                                                    private ThreadHelpBase,
                                                    public  ::cppu::WeakImplHelper1< ::com::sun::star::container::XContainerListener>
{
public:
                    ConfigurationAccess_ControllerFactory( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rServiceManager,const ::rtl::OUString& _sRoot,bool _bAskValue = false );
    virtual       ~ConfigurationAccess_ControllerFactory();

    void          readConfigurationData();
    void          updateConfigurationData();

    rtl::OUString getServiceFromCommandModule( const rtl::OUString& rCommandURL, const rtl::OUString& rModule ) const;
    rtl::OUString getValueFromCommandModule( const rtl::OUString& rCommandURL, const rtl::OUString& rModule ) const;
    void          addServiceToCommandModule( const rtl::OUString& rCommandURL, const rtl::OUString& rModule, const rtl::OUString& rServiceSpecifier );
    void          removeServiceFromCommandModule( const rtl::OUString& rCommandURL, const rtl::OUString& rModule );
    inline bool   hasValue() const { return m_bAskValue; }

    // container.XContainerListener
    virtual void SAL_CALL elementInserted( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL elementRemoved( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL elementReplaced( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);

    // lang.XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

private:
    struct ControllerInfo
    {
        rtl::OUString m_aImplementationName;
        rtl::OUString m_aValue;
        ControllerInfo(const ::rtl::OUString& _aImplementationName,const ::rtl::OUString& _aValue) : m_aImplementationName(_aImplementationName),m_aValue(_aValue){}
        ControllerInfo(){}
    };
    class MenuControllerMap : public std::hash_map< rtl::OUString,
                                                         ControllerInfo,
                                                         rtl::OUStringHash,
                                                         ::std::equal_to< ::rtl::OUString > >
    {
        inline void free()
        {
            MenuControllerMap().swap( *this );
        }
    };

    sal_Bool impl_getElementProps( const ::com::sun::star::uno::Any& aElement, rtl::OUString& aCommand, rtl::OUString& aModule, rtl::OUString& aServiceSpecifier,rtl::OUString& aValue ) const;

    rtl::OUString                     m_aPropCommand;
    rtl::OUString                     m_aPropModule;
    rtl::OUString                     m_aPropController;
    rtl::OUString                     m_aPropValue;
    rtl::OUString                     m_sRoot;
    MenuControllerMap                 m_aMenuControllerMap;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xConfigProvider;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >        m_xConfigAccess;
    sal_Bool                          m_bConfigAccessInitialized;
    bool                              m_bAskValue;
};

} // namespace framework
#endif // __FRAMEWORK_UIFACTORY_FACTORYCONFIGURATION_HXX_
