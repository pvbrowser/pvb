# create pipeline

#   create a hue lookup table
vtkLookupTable lut
# blue to red
    lut SetHueRange  0.66667 0.0
    lut Build

# create black to white colormap
vtkLookupTable lbw
#  black to white
    lbw SetHueRange  0 0.0
    lbw SetSaturationRange 0 0
    lbw SetValueRange 0 1

vtkStructuredPointsReader reader
    reader SetFileName "data1.vtk"
    reader Update

#reader needed otherwise range 0..1
    set valuerange [[reader GetOutput] GetScalarRange]
    set minv [lindex $valuerange 0]
    set maxv [lindex $valuerange 1]
#    puts "data range  $minv .. $maxv"

    set dims [[reader GetOutput] GetDimensions]
    set dim1 [lindex $dims 0]
    set dim2 [lindex $dims 1]
    set dim3 [lindex $dims 2]
#    puts "dim1 = $dim1 dim2 = $dim2"

# volgende echt nodig ...
# vtkStructuredPointsGeometryFilter plane
vtkImageDataGeometryFilter plane
    plane SetInput [reader GetOutput]
# SetExtent not needed ..

vtkWarpScalar warp
    warp SetInput [plane GetOutput]
    warp UseNormalOn
    warp SetNormal 0.0 0.0 1
    warp SetScaleFactor 1
vtkCastToConcrete caster
    caster SetInput [warp GetOutput]
vtkPolyDataNormals normals
    normals SetInput [caster GetPolyDataOutput]
    normals SetFeatureAngle 60
vtkPolyDataMapper planeMapper
    planeMapper SetInput [normals GetOutput]
    planeMapper SetLookupTable lut
    eval planeMapper SetScalarRange [[reader GetOutput] GetScalarRange]

vtkTransform transform
    transform Scale 0.02 0.02 0.02

vtkActor dataActor
    dataActor SetMapper planeMapper
    dataActor SetUserMatrix [transform GetMatrix]
#    dataActor SetOrigin -10.0 -10.0 0.0
#    dataActor SetScale  0.2

#vtkTextMapper textMapper
#    textMapper SetInput "hallo"
#
#vtkActor textActor
#    textActor SetMapper textMapper

# assign our actor to the renderer
#   renderer ResetCamera
#   renderer SetViewport 0.3 0.3 0.7 0.7
vtkRenderer renderer
renderer AddActor dataActor
renderer SetBackground 1 1 1
renWin AddRenderer renderer

#    renderer AddActor textActor
set cam1 [renderer GetActiveCamera]
#
# $cam1 Dolly 0.5
$cam1 ParallelProjectionOff
# $cam1 SetDistance 10.0
# $cam1 Zoom 0.5
