Unreal 5.4 project, based on the VR starter assets, with added support for logging, and visualizing VR with integrated eye-tracking data.

**Dependencies**:
- Unreal Engine 5.4
- [Varjo OpenXR plugin](https://fab.com/s/5761f1f00efa)
- [Visual Studio Integration Tool](https://fab.com/s/81403d127574)

**Usage**:
- Open project.
- Import / build level.
- Add TSVLogger object to level.
- Add VRLogger object to level.
- Set the Logger field on VRLogger to the TSVLogger instance.
- Add VRETLogger object to level.
- Set VRLogger field on VRETLogger to VRLogger instance.
- Set file prefix to desired log file name.
- Wear headset with eyetracking enabled (tested on Varjo and Meta Quest Pro).
- Set DebugSphere > Rendering > Visible to show/hide indication of gaze vectors intersection with level geometry.
- Run Level in VRPreview mode.
- Inspect log file in /Content/VRETLogs/

**Visualization of fixations**:
- Run FixationsVisualizationWidgetBlueprint. (Rightclick and select run in content browser)
- On first run, click Install Requirements to install required python libraries.
- Check that the name of the latest log file is in the Input Log File entry field.
- Click Calculate Fixations. Calls python library taken from [Llanes-Jurado et al. (2020)](https://www.mdpi.com/1424-8220/20/17/4956). Refer to paper for explanation of parameters.
- Wait!
- Check that a new file has been created under /Content/VRETLogs/withCalculatedFixations/, and its name is in the Processed Log File entry field.
- Click Visualize Fixations.
- Note that an ETVisualization object with blue spheres representing fixations as children appears in the level.
- Scrub the slider or play the level from the editor to replay the movement of the headset and appearance of fixations.
  

**Components**:
- VRETLogger
  - Blueprint class that checks that tracking is enabled, starts logging after a specified delay, and updates tracking data to log every tick.
  - Supports logging more data (focal depth, stereo gaze, pupil diameter, eyeblinks) than is currently exposed by the VarjoXR plugin.
- VRLogger
  - C++ class that registers tracking variables (columns) to log, updates values and checks for gaze vector intersections in the scene by calls to public function UpdateETData.
- TSVLogger
  - C++ class that handles logging of data organized in columns (variables) and rows (ticks) as tab separated text files.
  - Can log custom varibles (besides the tracking data) by calling AddEntry() (before StartLogging() is called in VRETLogging) and then one of the Update functions at every tick. All these functions are callable from blueprints. For an example, see LogTest blueprint.
 

