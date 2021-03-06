#
# Build PsFastXml
#

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/fastxml)

# Use generator expressions to set config specific preprocessor definitions
SET(PSFASTXML_COMPILE_DEFS 

	# Common to all configurations
	${PXSHARED_TVOS_COMPILE_DEFS};PX_FOUNDATION_DLL=0;

	$<$<CONFIG:debug>:${PXSHARED_TVOS_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PXSHARED_TVOS_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PXSHARED_TVOS_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PXSHARED_TVOS_RELEASE_COMPILE_DEFS};>
)

# include PsFastXml common
INCLUDE(../common/PsFastXml.cmake)