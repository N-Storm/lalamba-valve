#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=main.c light_ws2812.c interrupts.c timers.c saveload.c valve.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/light_ws2812.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/timers.o ${OBJECTDIR}/saveload.o ${OBJECTDIR}/valve.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/light_ws2812.o.d ${OBJECTDIR}/interrupts.o.d ${OBJECTDIR}/timers.o.d ${OBJECTDIR}/saveload.o.d ${OBJECTDIR}/valve.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/light_ws2812.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/timers.o ${OBJECTDIR}/saveload.o ${OBJECTDIR}/valve.o

# Source Files
SOURCEFILES=main.c light_ws2812.c interrupts.c timers.c saveload.c valve.c

# Pack Options 
PACK_COMPILER_OPTIONS=-I ${DFP_DIR}\include
PACK_COMMON_OPTIONS=-B ${DFP_DIR}\avr-gcc\dev\atmega8



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATmega8
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/light_ws2812.o: light_ws2812.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/light_ws2812.o.d 
	@${RM} ${OBJECTDIR}/light_ws2812.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/light_ws2812.o.d" -MT "${OBJECTDIR}/light_ws2812.o.d" -MT ${OBJECTDIR}/light_ws2812.o  -o ${OBJECTDIR}/light_ws2812.o light_ws2812.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/interrupts.o.d" -MT "${OBJECTDIR}/interrupts.o.d" -MT ${OBJECTDIR}/interrupts.o  -o ${OBJECTDIR}/interrupts.o interrupts.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/timers.o: timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timers.o.d 
	@${RM} ${OBJECTDIR}/timers.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/timers.o.d" -MT "${OBJECTDIR}/timers.o.d" -MT ${OBJECTDIR}/timers.o  -o ${OBJECTDIR}/timers.o timers.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/saveload.o: saveload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/saveload.o.d 
	@${RM} ${OBJECTDIR}/saveload.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/saveload.o.d" -MT "${OBJECTDIR}/saveload.o.d" -MT ${OBJECTDIR}/saveload.o  -o ${OBJECTDIR}/saveload.o saveload.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/valve.o: valve.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/valve.o.d 
	@${RM} ${OBJECTDIR}/valve.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS} -g -DDEBUG -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/valve.o.d" -MT "${OBJECTDIR}/valve.o.d" -MT ${OBJECTDIR}/valve.o  -o ${OBJECTDIR}/valve.o valve.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/light_ws2812.o: light_ws2812.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/light_ws2812.o.d 
	@${RM} ${OBJECTDIR}/light_ws2812.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/light_ws2812.o.d" -MT "${OBJECTDIR}/light_ws2812.o.d" -MT ${OBJECTDIR}/light_ws2812.o  -o ${OBJECTDIR}/light_ws2812.o light_ws2812.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/interrupts.o.d" -MT "${OBJECTDIR}/interrupts.o.d" -MT ${OBJECTDIR}/interrupts.o  -o ${OBJECTDIR}/interrupts.o interrupts.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/timers.o: timers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/timers.o.d 
	@${RM} ${OBJECTDIR}/timers.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/timers.o.d" -MT "${OBJECTDIR}/timers.o.d" -MT ${OBJECTDIR}/timers.o  -o ${OBJECTDIR}/timers.o timers.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/saveload.o: saveload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/saveload.o.d 
	@${RM} ${OBJECTDIR}/saveload.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/saveload.o.d" -MT "${OBJECTDIR}/saveload.o.d" -MT ${OBJECTDIR}/saveload.o  -o ${OBJECTDIR}/saveload.o saveload.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
${OBJECTDIR}/valve.o: valve.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/valve.o.d 
	@${RM} ${OBJECTDIR}/valve.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega8 ${PACK_COMPILER_OPTIONS} ${PACK_COMMON_OPTIONS}  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DDEBUG -DF_CPU=16000000 -Wall -MD -MP -MF "${OBJECTDIR}/valve.o.d" -MT "${OBJECTDIR}/valve.o.d" -MT ${OBJECTDIR}/valve.o  -o ${OBJECTDIR}/valve.o valve.c  -DXPRJ_default=$(CND_CONF)  $(COMPARISON_BUILD)  -flto -g -std=gnu11
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega8 ${PACK_COMMON_OPTIONS}  -D__MPLAB_DEBUGGER_SIMULATOR=1 -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="dist\${CND_CONF}\${IMAGE_TYPE}\src.${IMAGE_TYPE}.map"    -o dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  -flto -g -std=gnu11 $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_SIMULATOR=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group  -Wl,--warn-common -Wl,-u,vfprintf -lprintf_min
	
	
	${MP_CC_DIR}\\avr-objdump -h -S "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.lss"
	
	
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega8 ${PACK_COMMON_OPTIONS}  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="dist\${CND_CONF}\${IMAGE_TYPE}\src.${IMAGE_TYPE}.map"    -o dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_default=$(CND_CONF)  -flto -g -std=gnu11 $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,--end-group  -Wl,--warn-common -Wl,-u,vfprintf -lprintf_min
	${MP_CC_DIR}\\avr-objcopy -O ihex "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.hex"
	
	${MP_CC_DIR}\\avr-objdump -h -S "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "dist/${CND_CONF}/${IMAGE_TYPE}/src.${IMAGE_TYPE}.lss"
	
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
