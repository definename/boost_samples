# Find the WTL includes.

# This module will set the following variables in your project:
#
#	WTL_INCLUDE_DIRS, where to find atlapp.h
#	WTL_FOUND, If false, do not try to use WTL.

# The following cache should be set:
#
#	WTL_ROOT to the root directory of an WTL installation.
#

if (NOT WTL_ROOT)
	set(WTL_ROOT $ENV{WTL_ROOT})
endif()

find_path(WTL_INCLUDE_DIRS
	NAMES
		wtl/atlapp.h
	PATHS
		${WTL_ROOT}
	PATH_SUFFIXES
		include
	NO_DEFAULT_PATH
	DOC "WTL include directory"
	)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WTL DEFAULT_MSG WTL_INCLUDE_DIRS)

mark_as_advanced(WTL_INCLUDE_DIRS)