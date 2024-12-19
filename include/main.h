#pragma once

#ifndef __MAIN_H
#define __MAIN_H

#ifdef TARGET_DISCO_F429ZI

#define DEBUG_ON true

#else
#error "This class must be used with DISCO_F429ZI board only."
#endif // TARGET_DISCO_F429ZI

#endif // __MAIN_H
