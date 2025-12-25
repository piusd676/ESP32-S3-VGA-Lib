idf_component_register(
    SRCS "VGADisplayManager.cpp" "main.cpp" "VGAESP32S3.cpp" "VGARes.cpp"
    INCLUDE_DIRS "." 
)

target_add_binary_data(${COMPONENT_LIB} "output.raw" BINARY)
