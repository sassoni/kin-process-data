Usage: dataToPCDs kinect_file_config.xml


This is used to convert the data captured from one Kinect from depth maps to point clouds and remove the background.
(should do the background removal optional)

It reads the background data and the subject data specified in the config folder and saves colored point clouds
in the save folder specified. 

In the config file the following must be set:
<kinect_id> For identification purposes
<kinect_intrinsics> This is needed for a) projecting the depth to points and b) mapping the color to the cloud
<background_data> .data file (whole path) of the background 
<background_frames_no> no of frames to use in order to create the background model
<depth_data> .data subject depth data
<depth_info> .txt subject depth info. This is used to get the serial of each frame
<color_data> .data subject color data
<color_info> .txt subject color info. I think for the moment this is not used. We just pick the corresponding color frame based on depth frame index
<save_folder> Where the point clouds should be saved, must exist.
