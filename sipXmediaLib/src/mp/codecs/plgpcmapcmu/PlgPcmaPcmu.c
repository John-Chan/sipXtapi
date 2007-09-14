//  
// Copyright (C) 2007 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//
// Copyright (C) 2007 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Sergey Kostanbaev <Sergey DOT Kostanbaev AT sipez DOT com>

// APPLICATION INCLUDES
#include <mp/codecs/PlgDefsV1.h>

DECLARE_FUNCS_V1(sipxPcmu);
DECLARE_FUNCS_V1(sipxPcma);
// Declaring plugin contains two codecs
PLG_DOUBLE_CODECS(sipxPcmu, sipxPcma);