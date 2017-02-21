/* OpenSceneGraph example, osgdrawinstanced.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/
//
// This code is copyright (c) 2008 Skew Matrix Software LLC. You may use
// the code under the licensing terms described above.
//

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osgEarth/Random>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/Point>

#include <osgDB/WriteFile>

#include <iostream>

#define TEXTURE_DIM 2048


struct NotifyCameraPostDrawCallback : public osg::Camera::DrawCallback
{
    NotifyCameraPostDrawCallback(const std::string& message):
_message(message)
    {
    }

    virtual void operator () (osg::RenderInfo& renderInfo) const
    {                
        OE_NOTICE << _message << std::endl;
    }

    std::string _message;
};

void
createDAIGeometry( osg::Geometry& geom, int nInstances=1 )
{
    osg::Vec3Array* v = new osg::Vec3Array;
    v->resize( 1 );
    geom.setVertexArray( v );

    // Geometry for a single quad.
    (*v)[ 0 ] = osg::Vec3( 0.0, 0.0, 0.0 );

    // Use the DrawArraysInstanced PrimitiveSet and tell it to draw 1024 instances.
    geom.addPrimitiveSet( new osg::DrawArrays( GL_POINTS, 0, 1, nInstances ) );
    
    geom.setInitialBound(osg::BoundingBox(osg::Vec3(-5000.0, -5000.0, -5000.0), osg::Vec3(5000.0, 5000.0, 5000.0)));

}

osg::Texture2D* createPositionTexture()
{
    osgEarth::Random random;

    osg::Image* positionImage = new osg::Image;    
    positionImage->allocateImage(TEXTURE_DIM,TEXTURE_DIM,1, GL_RGBA, GL_FLOAT);
    positionImage->setInternalTextureFormat(GL_RGBA32F_ARB);
    GLfloat* ptr = reinterpret_cast<GLfloat*>( positionImage->data() );
    
    
    for (unsigned int i = 0; i < TEXTURE_DIM * TEXTURE_DIM; i++)
    {
        /*
        float x = -500.0 + random.next() * 1000.0;
        float y = -500.0 + random.next() * 1000.0;
        float z = -500.0 + random.next() * 1000.0;                
        */

        // Start in the center and eminate out.
        float x = 0.0;
        float y = 0.0;
        float z = 0.0;

        //OSG_NOTICE << x << ", " << y << ", " << z << std::endl;

        *ptr++ = x;
        *ptr++ = y;
        *ptr++ = z;
        // Random life
        *ptr++ = random.next();
    }
    
    osg::Texture2D* tex = new osg::Texture2D( positionImage );
    tex->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);
    tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
    return tex;      
}

osg::Texture2D* createDirectionTexture()
{
    osgEarth::Random random;

    osg::Image* positionDirection = new osg::Image;    
    positionDirection->allocateImage(TEXTURE_DIM,TEXTURE_DIM,1, GL_RGBA, GL_FLOAT);
    positionDirection->setInternalTextureFormat(GL_RGBA32F_ARB);
    GLfloat* ptr = reinterpret_cast<GLfloat*>( positionDirection->data() );

    float minTheta = 0.0;
    float maxTheta = 0.5f*osg::PI_4;
    float minPhi = 0.0;
    float maxPhi = 2*osg::PI;
        
    for (unsigned int i = 0; i < TEXTURE_DIM * TEXTURE_DIM; i++)
    {
        // Initial velocity
        float velocity = random.next() * 50.0;

        // Circle
        //float x = -0.5 + random.next();
        //float y = -0.5 + random.next();
        //float z = -0.5 + random.next();   
      
        /*
        float x = random.next();
        float y = random.next();
        float z = random.next();   
        */

       
        float theta = minTheta + (maxTheta - minTheta) * random.next();
        float phi = minPhi + (maxPhi - minPhi) * random.next();

        float x = velocity * sinf(theta) * cosf(phi);
        float y = velocity * sinf(theta) * sinf(phi);
        float z = velocity * cosf(theta);

        // Initial velocity
        float acceleration = random.next() * 100.0;        

        osg::Vec3 dir(x, y, z);     

        *ptr++ = dir.x();
        *ptr++ = dir.y();
        *ptr++ = dir.z();
        *ptr++ = acceleration;
    }
    
    osg::Texture2D* tex = new osg::Texture2D( positionDirection );
    tex->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);
    tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
    return tex;      
}


osg::Node* makeQuad(int width, int height, const osg::Vec4& color)
{
    osg::Geometry *geometry = new osg::Geometry;
    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->push_back(osg::Vec3(0,0,0));
    verts->push_back(osg::Vec3(width, 0, 0));
    verts->push_back(osg::Vec3(width,height,0));
    verts->push_back(osg::Vec3(0, height, 0));
    geometry->setVertexArray(verts);
    osg::Vec4Array* colors = new osg::Vec4Array();
    colors->push_back(color);
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    geometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, verts->size()));
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geometry);
    return geode;
}


std::string computeVert =
"void main() \n"
"{ \n"
"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \n"
"} \n";

std::string computeFrag =
"uniform sampler2D texturePosition; \n"
"uniform sampler2D textureVelocity; \n"
"uniform float osg_DeltaSimulationTime; \n"
"uniform float osg_SimulationTime; \n"
"uniform vec2 resolution;\n"

"uniform vec3 gravity;\n"

// Generate a pseudo-random value in the specified range:
"float\n"
"oe_random(float minValue, float maxValue, vec2 co)\n"
"{\n"
"    float t = fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n"
"    return minValue + t*(maxValue-minValue);\n"
"}\n"


"void main() \n"
"{ \n"
"   vec2 uv = gl_FragCoord.xy / resolution.xy;\n"

"   vec4 positionInfo = texture2D( texturePosition, uv );\n"    
"   vec4 velocityInfo = texture2D( textureVelocity, uv );\n"

"   vec3 position = positionInfo.xyz;\n"
"   float life = positionInfo.w;\n"
"   vec3 velocity = velocityInfo.xyz;\n"

// Apply various forces to compute a new velocity

// Gravity
//"   velocity = velocity + vec3(0.0, 0.0, -9.8) * osg_DeltaSimulationTime;\n"
"   velocity = velocity + gravity * osg_DeltaSimulationTime;\n"

// Compute the new position based on the velocity
"   position = position + velocity * osg_DeltaSimulationTime;\n"

// Compute the new velocity based on the acceleration
//"  velocity = velocity + acceleration * osg_DeltaSimulationTime;\n"

"   life -= osg_DeltaSimulationTime / 20.0f;\n"
// Reset particle
"   if (life < 0.0) {\n"
"       life = oe_random(0.0, 1.0, vec2(position.x, position.y));\n"
"       float initialVelocity = oe_random(50.0, 250.0, vec2(position.y, position.z));\n"
"       float x = oe_random(-0.5, 0.5, vec2(position.x, position.y));\n"
"       float y = oe_random(-0.5, 0.5, vec2(position.y, position.x));\n"
"       float z = 1.0;//oe_random(-0.5, 0.5, vec2(position.x, position.z));\n"
"       velocity = initialVelocity * normalize(vec3(x, y, z));\n"
"       position = vec3(0.0, 0.0, 0.0);\n"
"   }\n"


//  Write out the new position
//"   gl_FragColor = vec4(position, life);\n"
//"   gl_FragColor = vec4(position, velocity);\n"
"     gl_FragData[0] = vec4(position, life);\n"
"     gl_FragData[1] = vec4(velocity, 1.0);\n"
"} \n";


// Helper node that will run a fragment shader, taking one texture as input and writing to another texture.
// And then it flips on each frame to use the previous input.
class ComputeNode : public osg::Group
{
public:
    ComputeNode():
      _size(TEXTURE_DIM)
    {
        _inputPosition  = createPositionTexture();
        _outputPosition = createPositionTexture();
        _velocityInput = createDirectionTexture();
        _velocityOutput = createDirectionTexture();

        buildCamera();
    }

    osg::StateSet* createStateSet()
    {
        osg::Program* program = new osg::Program;
        program->addShader(new osg::Shader(osg::Shader::VERTEX, computeVert));
        program->addShader(new osg::Shader(osg::Shader::FRAGMENT, computeFrag));
        osg::StateSet* ss = new osg::StateSet;    
        ss->setAttributeAndModes(program);

        ss->addUniform(new osg::Uniform("texturePosition", 0 ));
        ss->addUniform(new osg::Uniform("textureVelocity", 1 ));

        ss->addUniform(new osg::Uniform( "resolution", osg::Vec2f(_size, _size)));

        // Use the input texture as texture 0
        ss->setTextureAttributeAndModes(0, _inputPosition.get(), osg::StateAttribute::ON);
        ss->setTextureAttributeAndModes(1, _velocityInput.get(), osg::StateAttribute::ON);

        return ss;
    }

    osg::Camera* createRTTCamera()
    {        
        osg::Camera* camera = new osg::Camera;

        camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set view
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);        

        // set viewport
        camera->setViewport(0,0, _size, _size );

        // set the camera to render before the main camera.
        camera->setRenderOrder(osg::Camera::PRE_RENDER);

        // tell the camera to use OpenGL frame buffer object where supported.
        camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

        // set up projection.
        camera->setProjectionMatrixAsOrtho2D(0.0, _size, 0.0, _size);
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

        // Make a full screen quad
        _quad = makeQuad(_size, _size, osg::Vec4(1,1,1,1));
        _quad->setCullingActive(false);
        _quad->setStateSet(createStateSet());        
        camera->addChild( _quad );

        return camera;
    }

    void buildCamera()
    {
        if (_camera.valid())
        {
            removeChild(_camera.get());
        }
        _camera = createRTTCamera();
        // Use the color buffer.
        //_camera->attach( osg::Camera::COLOR_BUFFER, _outputPosition, 0, 0, false );                
        _camera->attach( osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER0), _outputPosition);                
        _camera->attach( osg::Camera::BufferComponent(osg::Camera::COLOR_BUFFER1), _velocityOutput);                
        addChild(_camera.get());
    }

    void swap()
    {
        // Swap the positions
        osg::ref_ptr< osg::Texture2D > tmp = _inputPosition.get();
        _inputPosition = _outputPosition.get();
        _outputPosition = tmp.get();

        // Swap the velocities
        tmp = _velocityInput.get();
        _velocityInput = _velocityOutput.get();
        _velocityOutput = tmp.get();

        buildCamera();
    }

    osg::ref_ptr< osg::Texture2D > _inputPosition;
    osg::ref_ptr< osg::Texture2D > _outputPosition;
    osg::ref_ptr< osg::Texture2D > _velocityInput;
    osg::ref_ptr< osg::Texture2D > _velocityOutput;
    osg::ref_ptr< osg::Camera > _camera;
    osg::ref_ptr<osg::Node> _quad;
    
    unsigned int _size;
};


osg::Node* createBase(const osg::Vec3& center,float radius)
{
    int numTilesX = 10;
    int numTilesY = 10;

    float width = 2*radius;
    float height = 2*radius;

    osg::Vec3 v000(center - osg::Vec3(width*0.5f,height*0.5f,0.0f));
    osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
    osg::Vec3 dy(osg::Vec3(0.0f,height/((float)numTilesY),0.0f));

    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
    osg::Vec3Array* coords = new osg::Vec3Array;
    int iy;
    for(iy=0;iy<=numTilesY;++iy)
    {
        for(int ix=0;ix<=numTilesX;++ix)
        {
            coords->push_back(v000+dx*(float)ix+dy*(float)iy);
        }
    }

    //Just two colours - black and white.
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
    colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f)); // black

    osg::ref_ptr<osg::DrawElementsUShort> whitePrimitives = new osg::DrawElementsUShort(GL_QUADS);
    osg::ref_ptr<osg::DrawElementsUShort> blackPrimitives = new osg::DrawElementsUShort(GL_QUADS);

    int numIndicesPerRow=numTilesX+1;
    for(iy=0;iy<numTilesY;++iy)
    {
        for(int ix=0;ix<numTilesX;++ix)
        {
            osg::DrawElementsUShort* primitives = ((iy+ix)%2==0) ? whitePrimitives.get() : blackPrimitives.get();
            primitives->push_back(ix    +(iy+1)*numIndicesPerRow);
            primitives->push_back(ix    +iy*numIndicesPerRow);
            primitives->push_back((ix+1)+iy*numIndicesPerRow);
            primitives->push_back((ix+1)+(iy+1)*numIndicesPerRow);
        }
    }

    // set up a single normal
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));

    osg::Geometry* geom = new osg::Geometry;
    geom->setVertexArray(coords);

    geom->setColorArray(colors, osg::Array::BIND_PER_PRIMITIVE_SET);

    geom->setNormalArray(normals, osg::Array::BIND_OVERALL);

    geom->addPrimitiveSet(whitePrimitives.get());
    geom->addPrimitiveSet(blackPrimitives.get());

    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);

    return geode;
}


osg::StateSet*
createStateSet()
{
    osg::ref_ptr< osg::StateSet > ss = new osg::StateSet;

    // Create a vertex program that references the gl_InstanceID to
    // render each instance uniquely. gl_InstanceID will be in the range
    // 0 to numInstances-1 (1023 in our case).
    std::string vertexSource =
        "uniform sampler2D positionSampler; \n"
        "uniform float osg_SimulationTime; \n"
        "uniform mat4 osg_ViewMatrixInverse;\n"
        "uniform mat4 osg_ViewMatrix;\n"

        "uniform vec2 resolution;\n"

        "void main() \n"
        "{ \n"
            "mat4 modelView = gl_ModelViewMatrix;\n"

            "mat4 modelMatrix = gl_ModelViewMatrix * osg_ViewMatrixInverse;"

            // Using the instance ID, generate "texture coords" for this instance.
            "vec2 tC; \n"
            "float r = float(gl_InstanceID) / resolution.x; \n"
            "tC.s = fract( r ); tC.t = floor( r ) / resolution.y; \n"
            
            // Use the (scaled) tex coord to translate the position of the vertices.
            "vec4 posInfo = texture2D( positionSampler, tC );\n"
            "float life = posInfo.w;\n"
            "vec4 pos = gl_Vertex + vec4(posInfo.xyz, 0.0);\n"

            // Always red
            //"gl_FrontColor =  vec4(1.0, 0.0, 0.0, 1.0);\n"
            //"vec3 color = mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), clamp(velocity / 200.0, 0.0, 1.0));\n"
            "float alpha = clamp(life, 0.0, 1.0);\n"
            "vec3 color = mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), alpha);\n"
            "gl_FrontColor =  vec4(color, alpha);\n"


            //"gl_Position = gl_ProjectionMatrix * (pos + vec4(modelView[3].xyz, 0));\n"
            "gl_Position = gl_ModelViewProjectionMatrix * pos; \n"
            //"gl_Position = gl_ProjectionMatrix * modelView * pos;\n"
        "} \n";

    std::string fragSource =

        "void main() \n"
        "{ \n"
        "    gl_FragColor = gl_Color;\n"
        "} \n";

        
    osg::ref_ptr< osg::Program > program = new osg::Program();
    program->addShader( new osg::Shader(osg::Shader::VERTEX, vertexSource ));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragSource));

    ss->setAttribute( program.get(),
        osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );    

    osg::ref_ptr< osg::Uniform > positionUniform =
        new osg::Uniform( "positionSampler", 0 );
    ss->addUniform( positionUniform.get() );

     ss->addUniform(new osg::Uniform( "resolution", osg::Vec2f(TEXTURE_DIM, TEXTURE_DIM)));

    return( ss.release() );
}


struct GravityHandler : public osgGA::GUIEventHandler
{
    GravityHandler(osg::Uniform* uniform):
_uniform(uniform)
    {
    }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        if (ea.getEventType() == ea.KEYDOWN)
        {
            if (ea.getKey() == '+')
            {              
                osg::Vec3 value;
                _uniform->get(value);
                value += osg::Vec3(0,0,1);
                _uniform->set(value);
                return true;
            }
            else if (ea.getKey() == '-')
            {              
                osg::Vec3 value;
                _uniform->get(value);
                value -= osg::Vec3(0,0,1);
                _uniform->set(value);
                return true;
            }
        }
        return false;
    }

    osg::Uniform* _uniform;
};



int main( int argc, char **argv )
{
    osg::ArgumentParser arguments(&argc, argv);

    osg::Group* root = new osg::Group;

    root->addChild(createBase(osg::Vec3(0,0,-1000.0), 5000.0));

    // Add a compute node.
    ComputeNode* computeNode = new ComputeNode();
    root->addChild(computeNode);

   // Make a scene graph consisting of a single Geode, containing
    // a single Geometry, and a single PrimitiveSet.
    osg::ref_ptr< osg::Geode > geode = new osg::Geode;

    osg::ref_ptr< osg::Geometry > geom = new osg::Geometry;
    // Configure the Geometry for use with EXT_draw_arrays:
    // DL off and buffer objects on.
    geom->setUseDisplayList( false );
    geom->setUseVertexBufferObjects( true );
    
    createDAIGeometry( *geom, TEXTURE_DIM*TEXTURE_DIM );
    geode->addDrawable( geom.get() );
    geom->setCullingActive(false);
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    
    // Create a StateSet to render the instanced Geometry.
    osg::ref_ptr< osg::StateSet > ss = createStateSet();

    // Attatch the output of the compute node as the texture to feed the positions on the instanced geometry.
    ss->setTextureAttributeAndModes(0, computeNode->_outputPosition.get(), osg::StateAttribute::ON);

    geode->setStateSet( ss.get() );  

    geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Point(2.0));
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    root->addChild(geode);

    osgViewer::Viewer viewer(arguments);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setSceneData( root );

    viewer.setCameraManipulator(new osgGA::TrackballManipulator());

    osg::Uniform* gravity = new osg::Uniform("gravity", osg::Vec3(0.0, 0.0, -9.8));
    computeNode->getOrCreateStateSet()->addUniform(gravity);

    viewer.addEventHandler(new GravityHandler(gravity));

    
    double prevSimulationTime = viewer.getFrameStamp()->getSimulationTime();

    osgEarth::Random rand;
    
    while (!viewer.done())
    {
        viewer.frame();

        computeNode->swap();
        // Attatch the output of the compute node as the texture to feed the positions on the instanced geometry.
        ss->setTextureAttributeAndModes(0, computeNode->_outputPosition.get(), osg::StateAttribute::ON);
    }
}


