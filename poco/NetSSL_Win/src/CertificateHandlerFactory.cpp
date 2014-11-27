//
// CertificateHandlerFactory.cpp
//
// $Id: //poco/1.4/NetSSL_Win/src/CertificateHandlerFactory.cpp#1 $
//
// Library: NetSSL_Win
// Package: SSLCore
// Module:  CertificateHandlerFactory
//
// Copyright (c) 2006-2014, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/CertificateHandlerFactory.h"
#include "Poco/Net/SSLManager.h"


namespace Poco {
namespace Net {


CertificateHandlerFactory::CertificateHandlerFactory()
{
}


CertificateHandlerFactory::~CertificateHandlerFactory()
{
}


CertificateHandlerFactoryRegistrar::CertificateHandlerFactoryRegistrar(const std::string& name, CertificateHandlerFactory* pFactory)
{
	SSLManager::instance().certificateHandlerFactoryMgr().setFactory(name, pFactory);
}


CertificateHandlerFactoryRegistrar::~CertificateHandlerFactoryRegistrar()
{
}


} } // namespace Poco::Net