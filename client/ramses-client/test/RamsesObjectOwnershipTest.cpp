//  -------------------------------------------------------------------------
//  Copyright (C) 2015 BMW Car IT GmbH
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "ramses-client-api/AnimationSystemRealTime.h"
#include "ramses-client-api/AnimationSystem.h"
#include "ramses-client-api/RemoteCamera.h"
#include "ramses-client-api/PerspectiveCamera.h"
#include "ramses-client-api/OrthographicCamera.h"
#include "ramses-client-api/GeometryBinding.h"
#include "ramses-client-api/RenderGroup.h"
#include "ramses-client-api/RenderPass.h"
#include "ramses-client-api/BlitPass.h"
#include "ramses-client-api/MeshNode.h"
#include "ramses-client-api/Appearance.h"
#include "ramses-client-api/TextureSampler.h"
#include "ramses-client-api/RenderBuffer.h"
#include "ramses-client-api/RenderTarget.h"
#include "ramses-client-api/FloatArray.h"
#include "ramses-client-api/Vector2fArray.h"
#include "ramses-client-api/Vector3fArray.h"
#include "ramses-client-api/Vector4fArray.h"
#include "ramses-client-api/Texture2D.h"
#include "ramses-client-api/Texture3D.h"
#include "ramses-client-api/TextureCube.h"
#include "ramses-client-api/UInt16Array.h"
#include "ramses-client-api/EffectDescription.h"
#include "ramses-client-api/DataFloat.h"
#include "ramses-client-api/DataVector2f.h"
#include "ramses-client-api/DataVector3f.h"
#include "ramses-client-api/DataVector4f.h"
#include "ramses-client-api/DataMatrix22f.h"
#include "ramses-client-api/DataMatrix33f.h"
#include "ramses-client-api/DataMatrix44f.h"
#include "ramses-client-api/DataInt32.h"
#include "ramses-client-api/DataVector2i.h"
#include "ramses-client-api/DataVector3i.h"
#include "ramses-client-api/DataVector4i.h"
#include "ramses-client-api/StreamTexture.h"
#include "ramses-client-api/IndexDataBuffer.h"
#include "ramses-client-api/VertexDataBuffer.h"
#include "ramses-client-api/Texture2DBuffer.h"
#include "ramses-client-api/PickableObject.h"

#include "RamsesClientImpl.h"
#include "SceneImpl.h"
#include "ClientTestUtils.h"
#include "RamsesObjectTestTypes.h"

namespace ramses
{
    using namespace testing;

    template <typename ObjectType>
    class SceneOwnershipTest : public LocalTestClientWithSceneAndAnimationSystem, public testing::Test
    {
    public:
        SceneOwnershipTest() : LocalTestClientWithSceneAndAnimationSystem()
        {
        }

        void expectNoFrameworkObjectsAllocated()
        {
            m_creationHelper.destroyAdditionalAllocatedSceneObjects();
            const ramses_internal::IScene& scene = this->m_scene.impl.getIScene();

            for (ramses_internal::NodeHandle i(0); i < scene.getNodeCount(); ++i)
            {
                EXPECT_FALSE(scene.isNodeAllocated(i));
            }

            for (ramses_internal::CameraHandle i(0); i < scene.getCameraCount(); ++i)
            {
                EXPECT_FALSE(scene.isCameraAllocated(i));
            }

            for (ramses_internal::TransformHandle i(0); i < scene.getTransformCount(); ++i)
            {
                EXPECT_FALSE(scene.isTransformAllocated(i));
            }

            for (ramses_internal::RenderableHandle i(0); i < scene.getRenderableCount(); ++i)
            {
                EXPECT_FALSE(scene.isRenderableAllocated(i));
            }

            for (ramses_internal::RenderStateHandle i(0); i < scene.getRenderStateCount(); ++i)
            {
                EXPECT_FALSE(scene.isRenderStateAllocated(i));
            }

            for (ramses_internal::DataLayoutHandle i(0); i < scene.getDataLayoutCount(); ++i)
            {
                EXPECT_FALSE(scene.isDataLayoutAllocated(i));
            }

            for (ramses_internal::DataInstanceHandle i(0); i < scene.getDataInstanceCount(); ++i)
            {
                EXPECT_FALSE(scene.isDataInstanceAllocated(i));
            }

            for (ramses_internal::RenderPassHandle i(0); i < scene.getRenderPassCount(); ++i)
            {
                EXPECT_FALSE(scene.isRenderPassAllocated(i));
            }

            for (ramses_internal::RenderTargetHandle i(0); i < scene.getRenderTargetCount(); ++i)
            {
                EXPECT_FALSE(scene.isRenderTargetAllocated(i));
            }

            for (ramses_internal::TextureSamplerHandle i(0); i < scene.getTextureSamplerCount(); ++i)
            {
                EXPECT_FALSE(scene.isTextureSamplerAllocated(i));
            }
        }
    };

    template <typename ResourceType>
    class ClientOwnershipTest : public LocalTestClientWithScene, public testing::Test{};

    TYPED_TEST_SUITE(SceneOwnershipTest, SceneObjectTypes);
    TYPED_TEST_SUITE(ClientOwnershipTest, ClientObjectTypes);

    TYPED_TEST(SceneOwnershipTest, sceneObjectsAreOfTypeSceneObject)
    {
        const SceneObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        EXPECT_TRUE(obj->isOfType(ERamsesObjectType_SceneObject));
        EXPECT_TRUE(obj->isOfType(ERamsesObjectType_ClientObject));
    }

    TYPED_TEST(SceneOwnershipTest, sceneObjectsHaveReferenceToTheirScene)
    {
        const SceneObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        EXPECT_EQ(&this->m_scene.impl, &obj->impl.getSceneImpl());
        EXPECT_EQ(&this->m_scene.impl.getIScene(), &obj->impl.getIScene());
        EXPECT_EQ(&this->client.impl, &obj->impl.getClientImpl());
    }

    TYPED_TEST(SceneOwnershipTest, sceneContainsCreatedObject)
    {
        const RamsesObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        const RamsesObject* sn = this->m_scene.findObjectByName("objectName");
        ASSERT_TRUE(nullptr != sn);
        EXPECT_EQ(obj, sn);
        EXPECT_EQ(obj, this->m_constRefToScene.findObjectByName("objectName"));
    }

    // TODO Violin improve this test
    // The test is very ugly, because it checks that "a scene does not contain ANY LL object after
    // a specific HL object was destroyed. Instead, it should be testing that a scene does not
    // contain the LL components of the HL object which is being deleted
    TYPED_TEST(SceneOwnershipTest, sceneDoesNotContainDestroyedObject)
    {
        SceneObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        EXPECT_EQ(StatusOK, this->m_scene.destroy(*obj));
        const RamsesObject* sn = this->m_scene.findObjectByName("objectName");
        ASSERT_TRUE(nullptr == sn);

        this->expectNoFrameworkObjectsAllocated();
    }

    TYPED_TEST(SceneOwnershipTest, creatingAndDestroyingObjectsUpdatesStatisticCounter)
    {
        this->m_scene.impl.getStatisticCollection().nextTimeInterval(); //object number is updated by nextTimeInterval()
        ramses_internal::UInt32 initialNumber = this->m_scene.impl.getStatisticCollection().statObjectsNumber.getCounterValue();
        EXPECT_EQ(0u, this->m_scene.impl.getStatisticCollection().statObjectsCreated.getCounterValue());
        EXPECT_EQ(0u, this->m_scene.impl.getStatisticCollection().statObjectsDestroyed.getCounterValue());

        SceneObject* obj = &this->template createObject<TypeParam>("objectName");
        ramses_internal::UInt32 numberCreated = this->m_scene.impl.getStatisticCollection().statObjectsCreated.getCounterValue();
        EXPECT_LE(1u, numberCreated); //some types create multiple scene objects (e.g. RenderTarget)
        EXPECT_EQ(0u, this->m_scene.impl.getStatisticCollection().statObjectsDestroyed.getCounterValue());

        this->m_scene.impl.getStatisticCollection().nextTimeInterval();
        EXPECT_EQ(initialNumber + numberCreated, this->m_scene.impl.getStatisticCollection().statObjectsNumber.getCounterValue());

        this->m_scene.destroy(*obj);
        EXPECT_LE(1u, this->m_scene.impl.getStatisticCollection().statObjectsDestroyed.getCounterValue());

        this->m_scene.impl.getStatisticCollection().nextTimeInterval();
        EXPECT_GT(initialNumber + numberCreated, this->m_scene.impl.getStatisticCollection().statObjectsNumber.getCounterValue());
        EXPECT_LE(initialNumber, this->m_scene.impl.getStatisticCollection().statObjectsNumber.getCounterValue());
    }

    TYPED_TEST(ClientOwnershipTest, clientContainsCreatedObject)
    {
        const RamsesObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        const RamsesObject* sn = this->client.findObjectByName("objectName");
        ASSERT_TRUE(nullptr != sn);
        EXPECT_EQ(obj, sn);
    }

    TYPED_TEST(ClientOwnershipTest, clientDoesNotContainDestroyedObject)
    {
        RamsesObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        if (obj->getType() == ERamsesObjectType_Scene)
        {
            this->client.destroy(static_cast<Scene&>(*obj));
        }
        else
        {
            this->client.destroy(static_cast<Resource&>(*obj));
        }
        const RamsesObject* sn = this->client.findObjectByName("objectName");
        ASSERT_TRUE(nullptr == sn);
    }

    TYPED_TEST(SceneOwnershipTest, sceneObjectNameChanged)
    {
        RamsesObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        obj->setName("otherObjectName");
        RamsesObject* sn = this->m_scene.findObjectByName("otherObjectName");
        ASSERT_TRUE(nullptr != sn);
        EXPECT_EQ(obj, sn);
    }

    TYPED_TEST(ClientOwnershipTest, clientObjectsAreOfTypeClientObject)
    {
        const ClientObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        EXPECT_TRUE(obj->isOfType(ERamsesObjectType_ClientObject));
    }

    TYPED_TEST(ClientOwnershipTest, clientObjectsHaveReferenceToTheirClient)
    {
        const ClientObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        EXPECT_EQ(&this->client.impl, &obj->impl.getClientImpl());
    }

    TYPED_TEST(ClientOwnershipTest, clinetObjectNameChanged)
    {
        RamsesObject* obj = &this->template createObject<TypeParam>("objectName");
        ASSERT_TRUE(nullptr != obj);
        obj->setName("otherObjectName");
        RamsesObject* sn = this->client.findObjectByName("otherObjectName");
        ASSERT_TRUE(nullptr != sn);
        EXPECT_EQ(obj, sn);
    }

    TYPED_TEST(SceneOwnershipTest, sceneNotContainsDestroyedObject)
    {
        RamsesObject& obj = this->template createObject<TypeParam>("objectName");
        TypeParam& objTyped = static_cast<TypeParam&>(obj);

        this->m_scene.destroy(objTyped);
        const RamsesObject* sn = this->m_scene.findObjectByName("objectName");
        EXPECT_EQ(nullptr, sn);
    }

    TYPED_TEST(ClientOwnershipTest, clientNotContainsDestroyedObject)
    {
        RamsesObject& obj = this->template createObject<TypeParam>("objectName");
        TypeParam& objTyped = static_cast<TypeParam&>(obj);

        this->client.destroy(objTyped);
        const RamsesObject* sn = this->client.findObjectByName("objectName");
        EXPECT_EQ(nullptr, sn);
    }
}
