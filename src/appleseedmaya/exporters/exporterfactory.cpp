
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "appleseedmaya/exporters/exporterfactory.h"

// Standard headers.
#include <cassert>
#include <map>
#include <iostream>

// Maya headers.
#include <maya/MFnDagNode.h>

// appleseed.maya headers.
#include "appleseedmaya/exceptions.h"
#include "appleseedmaya/exporters/cameraexporter.h"
#include "appleseedmaya/exporters/lightexporter.h"
#include "appleseedmaya/exporters/meshexporter.h"


namespace asf = foundation;
namespace asr = renderer;

namespace
{

typedef std::map<std::string, NodeExporterFactory::CreateDagNodeExporterFn> CreateDagExporterMapType;
CreateDagExporterMapType gDagNodeExporters;

} // unnamed

MStatus NodeExporterFactory::initialize(const MString& pluginPath)
{
    CameraExporter::registerExporter();
    LightExporter::registerExporter();
    MeshExporter::registerExporter();
    return MS::kSuccess;
}

MStatus NodeExporterFactory::uninitialize()
{
    return MS::kSuccess;
}

void NodeExporterFactory::registerDagNodeExporter(
    const std::string&      mayaTypeName,
    CreateDagNodeExporterFn createFn)
{
    assert(createFn != 0);

    gDagNodeExporters[mayaTypeName] = createFn;

    std::cout << "NodeExporterFactory: registered dag node exporter for node: "
              << mayaTypeName << std::endl;
}

DagNodeExporter* NodeExporterFactory::createDagNodeExporter(
    const MDagPath& path,
    asr::Scene&     scene)
{
    MFnDagNode dagNodeFn(path);
    CreateDagExporterMapType::const_iterator it = gDagNodeExporters.find(dagNodeFn.typeName().asChar());

    if(it == gDagNodeExporters.end())
        throw NoExporterForNode();

    return it->second(path, scene);
}
