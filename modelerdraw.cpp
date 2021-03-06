#include "modelerdraw.h"
#include <FL/gl.h>
#include <FL/glut.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>

// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-1,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}

void drawCylinderWithTexture(double h, double r1, double r2, const char* dir)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

    _setupOpenGl();

    switch (mds->m_quality)
    {
    case HIGH:
        divisions = 32; break;
    case MEDIUM:
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2);
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n");
    }
    else
    {
        GLUquadricObj* gluq;

        // GLU will again do the work.  draw the sides of the cylinder. 
        glEnable(GL_TEXTURE_2D);
        Texture tex;
        tex.loadBMP_custom(dir);
        gluq = gluNewQuadric();
        gluQuadricDrawStyle(gluq, GLU_FILL);
        gluQuadricTexture(gluq, GL_TRUE);
        gluCylinder(gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric(gluq);
        glDisable(GL_TEXTURE_2D);

        if (r1 > 0.0)
        {
            // if the r1 end does not come to a point, draw a flat disk to
            //cover it up. 

            gluq = gluNewQuadric();
            gluQuadricDrawStyle(gluq, GLU_FILL);
            gluQuadricTexture(gluq, GL_TRUE);
            gluQuadricOrientation(gluq, GLU_INSIDE);
            gluDisk(gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric(gluq);
        }

        if (r2 > 0.0)
        {
            // if the r2 end does not come to a point, draw a flat disk to
            //cover it up. 

            // save the current matrix mode. 
            int savemode;
            glGetIntegerv(GL_MATRIX_MODE, &savemode);

            // translate the origin to the other end of the cylinder. 
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glTranslated(0.0, 0.0, h);

            // draw a disk centered at the new origin. 
            gluq = gluNewQuadric();
            gluQuadricDrawStyle(gluq, GLU_FILL);
            gluQuadricTexture(gluq, GL_TRUE);
            gluQuadricOrientation(gluq, GLU_OUTSIDE);
            gluDisk(gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric(gluq);

            // restore the matrix stack and mode. 
            glPopMatrix();
            glMatrixMode(savemode);
        }
    }

}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}

void drawTorus(GLdouble r1, GLdouble r2)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

    _setupOpenGl();

    switch (mds->m_quality)
    {
    case HIGH:
        divisions = 32; break;
    case MEDIUM:
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,
            "cone { bottom_radius=%f; top_radius=%f;\n", r1, r2);
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n");
    }
    else
    {
        divisions *= 6;

        for (int i = 0; i < divisions; i++){
            GLdouble curAngle = M_PI * 2.0 / divisions * i;
            GLdouble curOx, curOz;
            curOx = r1 * cos(curAngle);
            curOz = r1 * sin(curAngle);

            glPushMatrix();
            glTranslated(curOx, 0.0, curOz);
            glRotated(-curAngle / M_PI*180.0, 0.0, 1.0, 0.0);
            drawCylinder(M_PI * 2.0 / divisions * (r1 + r2)*1.02, r2, r2);
            glPopMatrix();
        }

    }
}

void drawTriangularPyramid(double r)
{
    double x1 = 0;
    double y1 = 0;
    double z1 = 0;
    double x2 = r / 2;
    double y2 = 0;
    double z2 = r * sqrt(3) / 2;
    double x3 = r;
    double y3 = 0;
    double z3 = 0;
    double x4 = r / 2;
    double y4 = r *  sqrt(6) / 3;
    double z4 = r * sqrt(3) / 6;
    drawTriangle(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    drawTriangle(x1, y1, z1, x2, y2, z2, x4, y4, z4);
    drawTriangle(x1, y1, z1, x3, y3, z3, x4, y4, z4);
    drawTriangle(x2, y2, z2, x3, y3, z3, x4, y4, z4);
}

void drawRectangularPyramid(double r, double h)
{
    drawTriangle(0, 0, 0, 0, 0, r, r, 0, r);
    drawTriangle(0, 0, 0, r, 0, r, r, 0, 0);
    drawTriangle(r / 2, h, r / 2, 0, 0, 0, 0, 0, r);
    drawTriangle(r / 2, h, r / 2, 0, 0, r, r, 0, r);
    drawTriangle(r / 2, h, r / 2, r, 0, r, r, 0, 0);
    drawTriangle(r / 2, h, r / 2, r, 0, 0, 0, 0, 0);
}

void drawTriangularPrism(double a, double b, double h, double theta)
{
    double sinT = sin(theta * M_PI / 180);
    double cosT = cos(theta * M_PI / 180);
    drawTriangle(0, 0, 0, a, 0, 0, b * cosT, 0, b * sinT);

    drawTriangle(0, 0, 0, b * cosT, 0, b * sinT, 0, h, 0);
    drawTriangle(b * cosT, 0, b * sinT, b * cosT, h, b * sinT, 0, h, 0);

    drawTriangle(b * cosT, 0, b * sinT, a, 0, 0, b * cosT, h, b * sinT);
    drawTriangle(a, 0, 0, a, h, 0, b * cosT, h, b * sinT);

    drawTriangle(0, 0, 0, 0, h, 0, a, 0, 0);
    drawTriangle(a, 0, 0, 0, h, 0, a, h, 0);

    drawTriangle(0, h, 0, b * cosT, h, b * sinT, a, h, 0);
}

void drawRectangularPrism(double r1, double r2, double h)
{
    double a = (r2 - r1)/2;
    double b = (r2 + r1)/2;
    drawTriangle(0, 0, 0, r2, 0, r2, 0, 0, r2);
    drawTriangle(0, 0, 0, r2, 0, 0, r2, 0, r2);

    drawTriangle(0, 0, 0, a, h, b, a, h, a);
    drawTriangle(0, 0, 0, 0, 0, r2, a, h, b);

    drawTriangle(0, 0, 0, a, h, a, b, h, a);
    drawTriangle(0, 0, 0, b, h, a, r2, 0, 0);

    drawTriangle(r2, 0, 0, b, h, a, b, h, b);
    drawTriangle(r2, 0, 0, b, h, b, r2, 0, r2);

    drawTriangle(0, 0, r2, b, h, b, a, h, b);
    drawTriangle(0, 0, r2, r2, 0, r2, b, h, b);

    drawTriangle(a, h, a, a, h, b, b, h, b);
    drawTriangle(a, h, a, b, h, b, b, h, a);
}

// draw

void drawHead(double angle, int level)
{
    // draw head
    setAmbientColor(.1f,.1f,.1f);
    setDiffuseColor(COLOR_GRAY);
    glPushMatrix();
    glScaled(2, 2, 2);
    if(level > 0)
    {
        drawBox(1,1,1);
    }

        setAmbientColor(.1f,.1f,.1f);
        setDiffuseColor(COLOR_YELLOW);
        // draw head right dec
        glPushMatrix();
        glTranslated(0.5,1,1);
        glRotated(20 + angle, 0.0, 0.0, 1.0);
        glScaled(3, 1, 1);
        glRotated(90, 1.0 ,0.0, 0.0);
        if (level >2)
        {
            drawTriangularPyramid(0.5);
        }
        glPopMatrix();
        // draw head left dec
        glPushMatrix();
        glTranslated(0.5,1,1);
        glRotated(-20 - angle, 0.0, 0.0, 1.0);
        glScaled(3, 1, 1);
        glRotated(-120, 0.0, 0.0, 1.0);
        glRotated(90, 1.0 ,0.0, 0.0);
        if (level >2)
        {
            drawTriangularPyramid(0.5);
        }
        
        glPopMatrix();

        setAmbientColor(.1f,.1f,.1f);
        setDiffuseColor(COLOR_GRAY);
        // draw neck
        glPushMatrix();
        glTranslated(0.5, 0, 0.5);
        glTranslated(-0.2, -0.3, -0.2);
        if (level > 1)
        {
            drawBox(0.4, 0.4, 0.4);
        }
        glPopMatrix();

        glPushMatrix();
        setAmbientColor(.1f,.1f,.1f);
        setDiffuseColor(COLOR_RED);
        glTranslated(0.5, 1.25, 1);
        glRotated(90, 1.0, 0.0, 0.0);
        drawTorus(0.15, 0.05);
        glRotated(-90, 1.0, 0.0, 0.0);

        setAmbientColor(.1f,.1f,.1f);
        setDiffuseColor(COLOR_YELLOW);
        glTranslated(0, -1, 0);
        glScaled(0.2, 0.2, 0.2);
        glutSolidDodecahedron();

        glPopMatrix();

    glPopMatrix();
}

void drawShoulder(double r, double h, int level)
{
    glPushMatrix();

    if (level > 0)
    {
        drawBox(r, r, r);
    }
    if (level > 1)
    {
        glRotated(-90, 1.0, 0.0, 0.0);
        drawRectangularPyramid(r, h);
        glRotated(90, 1.0, 0.0, 0.0);
        glTranslated(0, r, 0);
        drawRectangularPyramid(r, h);
        glTranslated(0, 0, r);
        glRotated(90, 1.0, 0.0, 0.0);
        drawRectangularPyramid(r, h);
        glRotated(-90, 1.0, 0.0, 0.0);
        glTranslated(r, 0, 0);
        glRotated(90, 0, 1, 0);
        glRotated(90, 1, 0, 0);
        drawRectangularPyramid(r, h);
    }
    glPopMatrix();
}


void drawLSystem(int depth, int num){
    if (depth == 1)
    {
        glPushMatrix();
        glBegin(GL_LINES);
          glVertex3f(0, 0, 0);
          glVertex3f(0, 2, 0);
        glEnd();
        glTranslated(0, 2, 0);
        drawSphere(0.1);
        glTranslated(0, -2, 0);
        glBegin(GL_LINES);
          glVertex3f(0, 2, 0);
          glVertex3f(2, 4, 0);
        glEnd();
        glTranslated(0, 2, 0);
        drawSphere(0.1);
        glTranslated(0, -2, 0);
        glBegin(GL_LINES);
          glVertex3f(0, 2, 0);
          glVertex3f(-2, 4, 0);
        glEnd();
        glTranslated(0, 2, 0);
        drawSphere(0.1);
        glTranslated(0, -2, 0);
        glPopMatrix();
    }
    else 
    {
        glPushMatrix();
        glBegin(GL_LINES);
          glVertex3f(0, 0, 0);
          glVertex3f(0, 2, 0);
        glEnd();
        glTranslated(0, 2, 0);
        drawSphere(0.1);
        glRotated(-60/depth, 0.0, 0.0, 1.0);
        drawLSystem(depth -1, num);
        glPopMatrix();
        glPushMatrix();
        glTranslated(0, 2, 0);
        drawSphere(0.1);
        glRotated(60/depth, 0.0, 0.0, 1.0);
        drawLSystem(depth - 1, num);
        glPopMatrix();

    }
}

Mat4d getModelViewMatrix()
{
    /**************************
    **
    **  GET THE OPENGL MODELVIEW MATRIX
    **
    **  Since OpenGL stores it's matricies in
    **  column major order and our library
    **  use row major order, we will need to
    **  transpose what OpenGL gives us before returning.
    **
    **  Hint:  Use look up glGetFloatv or glGetDoublev
    **  for how to get these values from OpenGL.
    **
    *******************************/

        GLdouble m[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, m);
        Mat4d matMV(m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15] );

        return matMV.transpose(); // convert to row major
}

