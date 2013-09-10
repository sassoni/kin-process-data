<h3>Usage: dataToPCDs kinect_file_config.xml</h3>


This is used to convert the data captured from one Kinect from depth maps to point clouds and remove the background.
(should do the background removal optional).

It reads the background data and the subject data specified in the config folder and saves colored point clouds
in the save folder specified. 

In the config file the following must be set:
<dl>
  <dt>kinect_id</dt>
  <dd>For identification purposes</dd>
  <dt>kinect_intrinsics</dt>
  <dd>This is needed for a) projecting the depth to points and b) mapping the color to the cloud</dd>
  <dt>background_data</dt>
  <dd>.data file (whole path) of the background </dd>
  <dt>background_frames_no</dt>
  <dd>no of frames to use in order to create the background model</dd>
  <dt>depth_data</dt>
  <dd>.data subject depth data</dd>
  <dt>depth_info</dt>
  <dd>.txt subject depth info. This is used to get the serial of each frame</dd>
  <dt>color_data</dt>
  <dd>.data subject color data</dd>
  <dt>color_info</dt>
  <dd>.txt subject color info. I think for the moment this is not used. We just pick the corresponding color frame based on depth frame index</dd>
  <dt>save_folder</dt>
  <dd>Where the point clouds should be saved, must exist.</dd>
</dl>

