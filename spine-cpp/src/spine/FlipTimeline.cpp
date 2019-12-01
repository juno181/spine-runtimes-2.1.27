/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated May 1, 2019. Replaces all prior versions.
 *
 * Copyright (c) 2013-2019, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS
 * INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifdef SPINE_UE4
#include "SpinePluginPrivatePCH.h"
#endif

#include <spine/FlipTimeline.h>

#include <spine/Skeleton.h>
#include <spine/Event.h>

#include <spine/Slot.h>
#include <spine/SlotData.h>
#include <spine/Bone.h>
#include <spine/BoneData.h>

using namespace spine;

RTTI_IMPL(FlipTimeline, TranslateTimeline)

FlipTimeline::FlipTimeline(int frameCount, bool x) : TranslateTimeline(frameCount) {
	_frames.setSize(frameCount << 1, 0);
	_x = x;
}

void FlipTimeline::apply(Skeleton &skeleton, float lastTime, float time, Vector<Event *> *pEvents, float alpha,
	MixBlend blend, MixDirection direction
) {
	Bone *boneP = skeleton._bones[_boneIndex];
	Bone &bone = *boneP;

	if (time < _frames[0]) {

		bone._flipX = bone._data._flipX != bone._flipX;
		bone._flipY = bone._data._flipY != bone._flipY;

		if (lastTime > time) FlipTimeline::apply(skeleton, lastTime, (float)INT_MAX, pEvents, alpha, blend, direction);
		return;
	} else if (lastTime > time) 
		lastTime = -1;

	//frameIndex = (time >= _frames[_framesCount - 2] ? _framesCount : binarySearch(_frames, _framesCount, time, 2)) - 2;
	int frame = time >= _frames[_frames.size() - 2] ? _frames.size() : Animation::binarySearch(_frames, time, ENTRIES) - 2;

	if (_x) 
		bone._flipX = _frames[frame + 1];
	else 
		bone._flipY = _frames[frame + 1];

	SP_UNUSED(blend);
	SP_UNUSED(alpha);
	SP_UNUSED(pEvents);
	SP_UNUSED(direction);
}

void FlipTimeline::setFrame(int frameIndex, float time, bool x) {
	frameIndex *= ENTRIES;
	_frames[frameIndex] = time;
	_frames[frameIndex + 1] = x;
}


int FlipTimeline::getPropertyId() {
	return ((int)(_x ? TimelineType_FlipX : TimelineType_FlipY) << 24) + _boneIndex;
}
