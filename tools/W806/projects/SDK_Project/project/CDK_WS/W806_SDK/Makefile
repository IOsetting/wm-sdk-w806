.PHONY: clean All

All:
	@echo "----------Building project:[ W806_SDK - BuildSet ]----------"
	@
	@make -f "W806_SDK.mk" MakeIntermediateDirs && make -f "W806_SDK.mk" -j 4
	@echo Executing Post Build commands ...
	@export CDKPath="D:/software/CDK" ProjectPath="D:/work_svn/WM_FAE/trunk/FAE_001/project/W806_mcu/code/wm_sdk_W806/tools/W806/projects/SDK_Project/project/CDK_WS/W806_SDK" && ../../../../../../../tools/W806/utilities/cdk_aft_build.sh;../../../../../../../tools/W806/utilities/aft_build_project.sh
	@echo Done

clean:
	@echo "----------Cleaning project:[ W806_SDK - BuildSet ]----------"
	@make -f  "W806_SDK.mk" clean
