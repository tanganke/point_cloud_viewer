install(
    TARGETS point_cloud_viewer_exe
    RUNTIME COMPONENT point_cloud_viewer_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
