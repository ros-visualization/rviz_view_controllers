/*
 * Copyright (c) 2009, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "fixed_orientation_ortho_view_controller.h"
#include "rviz/viewport_mouse_event.h"
#include "rviz/visualization_manager.h"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <OGRE/OgreViewport.h>

#include <ogre_tools/shape.h>
#include <ogre_tools/orthographic.h>

#include <stdint.h>
#include <sstream>

namespace rviz
{

FixedOrientationOrthoViewController::FixedOrientationOrthoViewController(VisualizationManager* manager, const std::string& name, Ogre::SceneNode* target_scene_node)
: ViewController(manager, name, target_scene_node)
, scale_(10.0f)
, angle_( 0 )
{
}

FixedOrientationOrthoViewController::~FixedOrientationOrthoViewController()
{
}

void FixedOrientationOrthoViewController::reset()
{
  scale_ = 10;
  angle_ = 0;
  setPosition( Ogre::Vector3( 0, 0, 0 ));
}

void FixedOrientationOrthoViewController::handleMouseEvent(ViewportMouseEvent& event)
{
  bool moved = false;

  if( event.type == QEvent::MouseMove )
  {
    int32_t diff_x = event.x - event.last_x;
    int32_t diff_y = event.y - event.last_y;

    if( event.left() && !event.shift() )
    {
      angle_ -= -diff_x * 0.005;
      orientCamera();
    }
    else if( event.middle() || ( event.shift() && event.left() ))
    {
      move( -diff_x / scale_, diff_y / scale_ );
    }
    else if( event.right() )
    {
      scale_ *= 1.0 - diff_y * 0.01;
    }

    moved = true;
  }

  if ( event.wheel_delta != 0 )
  {
    int diff = event.wheel_delta;
    scale_ *= 1.0 - (-diff) * 0.001;

    moved = true;
  }

  if (moved)
  {
    manager_->queueRender();
  }
}

void FixedOrientationOrthoViewController::orientCamera()
{
  camera_->setOrientation( Ogre::Quaternion( Ogre::Radian( angle_ ), Ogre::Vector3::UNIT_Z ));
}

void FixedOrientationOrthoViewController::onActivate()
{
  camera_->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  camera_->setFixedYawAxis(false);
  setPosition( camera_->getPosition() );
  orientCamera();
}

void FixedOrientationOrthoViewController::onDeactivate()
{
  camera_->setCustomProjectionMatrix(false);
}

void FixedOrientationOrthoViewController::onUpdate(float dt, float ros_dt)
{
  updateCamera();
}

void FixedOrientationOrthoViewController::lookAt( const Ogre::Vector3& point )
{
  setPosition( point - target_scene_node_->getPosition() );
}

void FixedOrientationOrthoViewController::onTargetFrameChanged(const Ogre::Vector3& old_reference_position, const Ogre::Quaternion& old_reference_orientation)
{
  move( old_reference_position.x - reference_position_.x,
        old_reference_position.y - reference_position_.y );
}

void FixedOrientationOrthoViewController::updateCamera()
{
  orientCamera();

  float width = camera_->getViewport()->getActualWidth();
  float height = camera_->getViewport()->getActualHeight();

  Ogre::Matrix4 proj;
  ogre_tools::buildScaledOrthoMatrix( proj, -width / scale_ / 2, width / scale_ / 2, -height / scale_ / 2, height / scale_ / 2,
                                      camera_->getNearClipDistance(), camera_->getFarClipDistance() );
  camera_->setCustomProjectionMatrix(true, proj);
}

void FixedOrientationOrthoViewController::setPosition( const Ogre::Vector3& pos_rel_target )
{
  // For Z, we use an arbitrary large number smaller than camera's
  // far-clip-distance (100k).  Any objects above it will not show up.
  camera_->setPosition( pos_rel_target.x, pos_rel_target.y, 10000 );
}

void FixedOrientationOrthoViewController::move( float x, float y )
{
  camera_->moveRelative( Ogre::Vector3( x, y, 0 ));
}

void FixedOrientationOrthoViewController::fromString(const std::string& str)
{
  std::istringstream iss(str);

  iss >> scale_;
  iss.ignore();

  Ogre::Vector3 vec;
  iss >> vec.x;
  iss.ignore();
  iss >> vec.y;
  iss.ignore();
  setPosition(vec);

  iss >> angle_;
}

std::string FixedOrientationOrthoViewController::toString()
{
  std::ostringstream oss;
  oss << scale_ << " " << camera_->getPosition().x << " " << camera_->getPosition().y << " " << angle_;
  return oss.str();
}

}
