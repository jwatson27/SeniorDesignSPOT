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
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED="../../Microchip/Usb/HID Device Driver/usb_function_hid.c" ../../Microchip/Usb/usb_device.c ../usb_descriptors.c ../main.c "../../Microchip/BMA150 driver/BMA150.c" ../../Microchip/mTouch/mtouch.c "../../Microchip/OLED driver/oled.c" "../../Microchip/Soft Start/soft_start.c"

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1141713355/usb_function_hid.o ${OBJECTDIR}/_ext/713069093/usb_device.o ${OBJECTDIR}/_ext/1472/usb_descriptors.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/809689491/BMA150.o ${OBJECTDIR}/_ext/792076913/mtouch.o ${OBJECTDIR}/_ext/2061924019/oled.o ${OBJECTDIR}/_ext/238584075/soft_start.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1141713355/usb_function_hid.o.d ${OBJECTDIR}/_ext/713069093/usb_device.o.d ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/809689491/BMA150.o.d ${OBJECTDIR}/_ext/792076913/mtouch.o.d ${OBJECTDIR}/_ext/2061924019/oled.o.d ${OBJECTDIR}/_ext/238584075/soft_start.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1141713355/usb_function_hid.o ${OBJECTDIR}/_ext/713069093/usb_device.o ${OBJECTDIR}/_ext/1472/usb_descriptors.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/809689491/BMA150.o ${OBJECTDIR}/_ext/792076913/mtouch.o ${OBJECTDIR}/_ext/2061924019/oled.o ${OBJECTDIR}/_ext/238584075/soft_start.o

# Source Files
SOURCEFILES=../../Microchip/Usb/HID Device Driver/usb_function_hid.c ../../Microchip/Usb/usb_device.c ../usb_descriptors.c ../main.c ../../Microchip/BMA150 driver/BMA150.c ../../Microchip/mTouch/mtouch.c ../../Microchip/OLED driver/oled.c ../../Microchip/Soft Start/soft_start.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J50
MP_PROCESSOR_OPTION_LD=18f46j50
MP_LINKER_DEBUG_OPTION=  -u_DEBUGSTACK
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1141713355/usb_function_hid.o: ../../Microchip/Usb/HID\ Device\ Driver/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1141713355" 
	@${RM} ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o   "../../Microchip/Usb/HID Device Driver/usb_function_hid.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1141713355/usb_function_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/713069093/usb_device.o: ../../Microchip/Usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713069093" 
	@${RM} ${OBJECTDIR}/_ext/713069093/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/713069093/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/713069093/usb_device.o   ../../Microchip/Usb/usb_device.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/713069093/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713069093/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/usb_descriptors.o: ../usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/usb_descriptors.o   ../usb_descriptors.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/809689491/BMA150.o: ../../Microchip/BMA150\ driver/BMA150.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/809689491" 
	@${RM} ${OBJECTDIR}/_ext/809689491/BMA150.o.d 
	@${RM} ${OBJECTDIR}/_ext/809689491/BMA150.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/809689491/BMA150.o   "../../Microchip/BMA150 driver/BMA150.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/809689491/BMA150.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/809689491/BMA150.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/792076913/mtouch.o: ../../Microchip/mTouch/mtouch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/792076913" 
	@${RM} ${OBJECTDIR}/_ext/792076913/mtouch.o.d 
	@${RM} ${OBJECTDIR}/_ext/792076913/mtouch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/792076913/mtouch.o   ../../Microchip/mTouch/mtouch.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/792076913/mtouch.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/792076913/mtouch.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/2061924019/oled.o: ../../Microchip/OLED\ driver/oled.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2061924019" 
	@${RM} ${OBJECTDIR}/_ext/2061924019/oled.o.d 
	@${RM} ${OBJECTDIR}/_ext/2061924019/oled.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/2061924019/oled.o   "../../Microchip/OLED driver/oled.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/2061924019/oled.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2061924019/oled.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/238584075/soft_start.o: ../../Microchip/Soft\ Start/soft_start.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/238584075" 
	@${RM} ${OBJECTDIR}/_ext/238584075/soft_start.o.d 
	@${RM} ${OBJECTDIR}/_ext/238584075/soft_start.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG  -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/238584075/soft_start.o   "../../Microchip/Soft Start/soft_start.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/238584075/soft_start.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/238584075/soft_start.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/_ext/1141713355/usb_function_hid.o: ../../Microchip/Usb/HID\ Device\ Driver/usb_function_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1141713355" 
	@${RM} ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o   "../../Microchip/Usb/HID Device Driver/usb_function_hid.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1141713355/usb_function_hid.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1141713355/usb_function_hid.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/713069093/usb_device.o: ../../Microchip/Usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/713069093" 
	@${RM} ${OBJECTDIR}/_ext/713069093/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/713069093/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/713069093/usb_device.o   ../../Microchip/Usb/usb_device.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/713069093/usb_device.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/713069093/usb_device.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/usb_descriptors.o: ../usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/usb_descriptors.o   ../usb_descriptors.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/usb_descriptors.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/usb_descriptors.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/809689491/BMA150.o: ../../Microchip/BMA150\ driver/BMA150.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/809689491" 
	@${RM} ${OBJECTDIR}/_ext/809689491/BMA150.o.d 
	@${RM} ${OBJECTDIR}/_ext/809689491/BMA150.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/809689491/BMA150.o   "../../Microchip/BMA150 driver/BMA150.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/809689491/BMA150.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/809689491/BMA150.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/792076913/mtouch.o: ../../Microchip/mTouch/mtouch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/792076913" 
	@${RM} ${OBJECTDIR}/_ext/792076913/mtouch.o.d 
	@${RM} ${OBJECTDIR}/_ext/792076913/mtouch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/792076913/mtouch.o   ../../Microchip/mTouch/mtouch.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/792076913/mtouch.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/792076913/mtouch.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/2061924019/oled.o: ../../Microchip/OLED\ driver/oled.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2061924019" 
	@${RM} ${OBJECTDIR}/_ext/2061924019/oled.o.d 
	@${RM} ${OBJECTDIR}/_ext/2061924019/oled.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/2061924019/oled.o   "../../Microchip/OLED driver/oled.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/2061924019/oled.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2061924019/oled.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/238584075/soft_start.o: ../../Microchip/Soft\ Start/soft_start.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/238584075" 
	@${RM} ${OBJECTDIR}/_ext/238584075/soft_start.o.d 
	@${RM} ${OBJECTDIR}/_ext/238584075/soft_start.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I".." -I"../../Microchip/BMA150 driver" -I"../../Microchip/Include" -I"../../Microchip/Include/USB" -I"../../Microchip/OLED driver" -I"../../Microchip/Soft Start" -I"../../Microchip/Usb" -I"../../Microchip/mTouch" -I"." -ms -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/238584075/soft_start.o   "../../Microchip/Soft Start/soft_start.c" 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/238584075/soft_start.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/238584075/soft_start.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../rm18f46j50_g.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\rm18f46j50_g.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"$(BINDIR_)$(TARGETBASE).map" -w -l"../../../../MPLAB C18/lib" -l"."  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../rm18f46j50_g.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\rm18f46j50_g.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"$(BINDIR_)$(TARGETBASE).map" -w -l"../../../../MPLAB C18/lib" -l"."  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/HID_Demo_Test.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
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
