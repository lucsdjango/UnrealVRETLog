import pathlib
import pandas as pd
import os
from IDT_alg_VR_centred import IDTVR
import unreal
import numpy as np

eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
levelLibrary = unreal.EditorLevelLibrary()
assetLibrary = unreal.EditorAssetLibrary()

def deleteExistingVisualizations():

    visActors = unreal.GameplayStatics().get_all_actors_of_class( levelLibrary.get_editor_world(),assetLibrary.load_blueprint_class('/Game/ETVisualization.ETVisualization'))
    for visActor in visActors:
        eas.destroy_actor(visActor)
        
    fixActors = unreal.GameplayStatics().get_all_actors_of_class( levelLibrary.get_editor_world(),assetLibrary.load_blueprint_class('/Game/Fixation.Fixation'))
    for fixActor in fixActors:
        eas.destroy_actor(fixActor)

def readETData(fileName, time_th, disp_th, freq_th, useFocal):

    # delete existing fixation objects in scene
    print("Time threshold: %1f dispersion threshold: %2f freq threshold: %3f" % (time_th, disp_th, freq_th)) 
    
    deleteExistingVisualizations()
    
    df_et = pd.read_csv(unreal.Paths.project_content_dir() + "/VRETLogs/" + fileName, sep="\t",index_col=False )
    
    df_et = df_et[(df_et["Valid"] == "T")]
    
    
    start_time = df_et["time"].iloc[0]
    print(start_time)
    df_et["time"] = df_et["time"] - start_time
    
    
    
    df_et[['head_pose_x','head_pose_y','head_pose_z']] = df_et['GazeOrigin'].str.split(',',expand=True).astype(float)
    if (useFocal):
        df_et[['et_x','et_y','et_z']] = df_et['FixPoint'].str.split(',',expand=True).astype(float)
    else:
        df_et[['et_x','et_y','et_z']] = df_et['HitPos'].str.split(',',expand=True).astype(float)

    print("Shape", df_et.shape)
    print("Columns", df_et.columns)
    
    #IDTVR(time_th=0.15, disp_th=1, freq_th=50)
    
    idt_vr = IDTVR(time_th=time_th, disp_th=disp_th, freq_th=freq_th)

    df_et_w_fixations = idt_vr.fit_compute(df_et, time="time",)
    df_et_w_fixations['diff'] = df_et_w_fixations['class_disp'].diff()
    df_et_w_fixations['newFixation'] = df_et_w_fixations['diff'].abs() #df_et_w_fixations['diff'] == -1).astype(int)
    df_et_w_fixations['fixationIdx'] = df_et_w_fixations['newFixation'].cumsum()
    
    df_et_w_fixations = df_et_w_fixations.iloc[1:]
    
    basename, ext = fileName.rsplit(".", maxsplit=1)
    new_file_name =  basename + "_w_fixations." + ext 
      
    df_et_w_fixations.to_csv(unreal.Paths.project_content_dir() + "/VRETLogs/" + new_file_name, sep="\t") 
    
    return new_file_name

    
    
def visualizeFixations(fileName):
 
    deleteExistingVisualizations()
 
    visActor = levelLibrary.spawn_actor_from_class(assetLibrary.load_blueprint_class('/Game/ETVisualization.ETVisualization'), unreal.Vector(0,0,0))
    
    times = visActor.get_editor_property('times')
    gazePositions = visActor.get_editor_property('GazePositions')
    headPositions = visActor.get_editor_property('HeadPositions')
    
    fixations = visActor.get_editor_property('Fixations')   
    
    df_et_w_fixations = pd.read_csv(unreal.Paths.project_content_dir() + "/VRETLogs/" + fileName, sep="\t",index_col=False )
    print("Shape", df_et_w_fixations.shape)
    print("Columns", df_et_w_fixations.columns)
    
    
    for index, row in df_et_w_fixations.iterrows():
        times.append(row['time'])
        gazePositions.append(unreal.Vector(row['et_x'],row['et_y'],row['et_z']))
        headPositions.append(unreal.Vector(row['head_pose_x'],row['head_pose_y'],row['head_pose_z']))
        
    
    
    saccsAndFixes = df_et_w_fixations[(df_et_w_fixations["diff"] != 0)]
    saccsAndFixes['fixOrSacc'] = np.where(saccsAndFixes['diff'] > 0, "Saccade", "Fixation")
    saccsAndFixes['duration'] = 0-saccsAndFixes['time'].diff(-1) # time difference next row
    
    df2 = df_et_w_fixations[['fixationIdx','et_x', 'et_y', 'et_z']].copy()
    #df2['fixationIdx'] = df2['fixationIdx'].astype('category',copy=False)
    
    meanFixations = df2.groupby('fixationIdx', as_index=False).mean()
    print(meanFixations)
    
    for index, row in meanFixations.iterrows():
        saccsAndFixes['et_x'][index] = row['et_x']
        saccsAndFixes['et_y'][index] = row['et_y']
        saccsAndFixes['et_z'][index] = row['et_z']
    
    saccsAndFixes['dX'] = 0-saccsAndFixes['et_x'].diff(-1) 
    saccsAndFixes['dY'] = 0-saccsAndFixes['et_y'].diff(-1) 
    saccsAndFixes['dZ'] = 0-saccsAndFixes['et_z'].diff(-1) 
    saccsAndFixes['distance'] = np.sqrt(saccsAndFixes['dX']**2 + saccsAndFixes['dY']**2 + saccsAndFixes['dZ']**2)
    
    saccsAndFixes.to_csv(unreal.Paths.project_content_dir() + "/VRETLogs/" + "hej.tsv", sep="\t") 
    
    for index, row in saccsAndFixes.iterrows():
        if (row['fixOrSacc'] == "Saccade"):
            continue
        fixActor = levelLibrary.spawn_actor_from_class(assetLibrary.load_blueprint_class('/Game/Fixation.Fixation'), unreal.Vector(row['et_x'],row['et_y'],row['et_z']))
        fixActor.set_actor_scale3d(unreal.Vector(row['duration'],row['duration'],row['duration']).multiply_float(0.5))
        fixActor.attach_to_actor(visActor, visActor.get_actor_label(), unreal.AttachmentRule.KEEP_RELATIVE, unreal.AttachmentRule.KEEP_RELATIVE, unreal.AttachmentRule.KEEP_RELATIVE, False)
        fixActor.set_editor_property("Time", row['time'])
        fixActor.set_editor_property("Duration", row['duration'])
        fixations.append(fixActor)
        
    print("Shape", meanFixations.shape)
    
    #print("Shape", saccsAndFixes.shape)
    #print("Columns", saccsAndFixes.columns)
    #print(meanFixations['et_x'])
    return visActor

def scrub(value, visActor):


    
    
    
    
    times = visActor.get_editor_property('times')
    gazePositions = visActor.get_editor_property('GazePositions')
    headPositions = visActor.get_editor_property('HeadPositions')
    
    idx = len(times) * value;
    
    
    
    visActor.call_method("SetHeadAndGazePos", args=(idx,))
    visActor.call_method("UpdateVisibility", args=(times[idx],))
    
    
    levelLibrary.editor_invalidate_viewports()
    
    '''
    children =  visActor.get_all_child_actors(True)
    for a in children:
        actorLabel = a.get_actor_label()
        print(actorLabel)
        if (actorLabel == 'head'):
            a.set_actor_location(headPositions[idx], False, False)
        elif (actorLabel == 'gaze'):
            a.set_actor_location(gazePositions[idx], False, False)
    '''
    #head = visActor.get_editor_property('head')
    #gaze = visActor.get_editor_property('gaze')
    
    
    
    #head.child_actor.set_actor_location(headPositions[i], False, False)
    #gaze.child_actor.set_actor_location(gazePositions[i], False, False)
    
    
    
    