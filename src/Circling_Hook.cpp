#include "Circling_Hook.h"
#include "RE/CombatBehaviorNodesMovement.h"
#include "Util.h"

namespace CombatPathing
{
	static constexpr char ENABLE_CIRCLING_GV[] = "CPR_EnableCircling",
						  CIRCLING_MIN_DIST_GV[] = "CPR_CirclingDistMin", CIRCLING_MAX_DIST_GV[] = "CPR_CirclingDistMax",
						  CIRCLING_MIN_ANG_GV[] = "CPR_CirclingAngleMin", CIRCLING_MAX_ANG_GV[] = "CPR_CirclingAngleMax";

	static float GetCircleChance(RE::Character* a_character)
	{
		//sub_140845070
		return _generic_foo<49720, float, RE::Character*>(a_character);
	}

	float CirclingChanceHook::GetCirclingChance(const float a_circleMult, const float a_minChance, const float a_maxChance)
	{
		auto me = CombatAI__get_me();
		auto he = CombatAI__get_he();
		if (me && he && me->combatController && me->combatController->combatStyle) {
			bool enableCircling;
			if (me->GetGraphVariableBool(ENABLE_CIRCLING_GV, enableCircling) && enableCircling) {
				float circlingDistMin, circlingDistMax;
				if (me->GetGraphVariableFloat(CIRCLING_MIN_DIST_GV, circlingDistMin) && me->GetGraphVariableFloat(CIRCLING_MAX_DIST_GV, circlingDistMax)) {
					auto optimalWeapRange = GetEquippementRange(me->combatController->inventory);
					auto maxWeapRange = GetEquippementRange(me->combatController->inventory, true);
					auto distance = me->GetPosition().GetDistance(he->GetPosition()) - he->GetBoundRadius();
					circlingDistMin += circlingDistMin > 0.f ? optimalWeapRange : 0.f;
					circlingDistMax += maxWeapRange;

					if (distance < circlingDistMin || distance > circlingDistMax)
						return std::max(0.1f, a_minChance);  //The chance must be a bit greater than zero, ohterwise NPC would be stucked by barriers.
				}
			}
		}

		return _GetCirclingChance(a_circleMult, a_minChance, a_maxChance);
	}

	RE::NodeArray& AdvanceToCircleHook::PushBackNode(RE::NodeArray& a_master, RE::NodeArray& a_target)
	{
		auto nodeCirlce = RE::NodeCloseMovementCircleDistant::createnew();
		if (nodeCirlce) {
			NodeArray array;
			RE::TreeCtors_extradata extraData;
			extraData.func1 = ShouldCircle;
			extraData.func2 = nullptr;

			a_master = pushback_parentof(a_master, wrap_to_conditional_2(array, "CPR Circle Distant", &extraData, nodeCirlce));
		}

		return _PushBackNode(a_master, a_target);
	}

	bool AdvanceToCircleHook::ShouldCircle(void* a_context)
	{
		auto me = CombatAI__get_me();
		auto he = CombatAI__get_he();
		if (me && he && me->combatController && me->combatController->combatStyle) {
			bool enableCircling;
			if (me->GetGraphVariableBool(ENABLE_CIRCLING_GV, enableCircling) && enableCircling && IsMeleeOnly(me)) {
				float circlingDistMin, circlingDistMax;
				if (me->GetGraphVariableFloat(CIRCLING_MIN_DIST_GV, circlingDistMin) && me->GetGraphVariableFloat(CIRCLING_MAX_DIST_GV, circlingDistMax)) {
					auto optimalWeapRange = GetEquippementRange(me->combatController->inventory);
					auto maxWeapRange = GetEquippementRange(me->combatController->inventory, true);
					auto distance = me->GetPosition().GetDistance(he->GetPosition()) - he->GetBoundRadius();
					circlingDistMin += circlingDistMin > 0.f ? optimalWeapRange : 0.f;
					circlingDistMax += maxWeapRange;
					if (distance >= circlingDistMin && distance <= circlingDistMax) {
						auto chance = GetCircleChance(me);
						return Random::get(0.f, 1.0f) <= chance ? true : false;
					}
				}
			}
		}

		return false;
	}

	float CircleAngleHook1::RescaleCircleAngle(const float a_circleMult, const float a_minAnlge, const float a_maxAngle)
	{
		auto me = CombatAI__get_me();
		if (me && me->combatController && me->combatController->combatStyle) {
			bool enableCircling;
			if (me->GetGraphVariableBool(ENABLE_CIRCLING_GV, enableCircling) && enableCircling && IsMeleeOnly(me)) {
				float circlingAngleMin, circlingAngleMax;
				if (me->GetGraphVariableFloat(CIRCLING_MIN_ANG_GV, circlingAngleMin) && me->GetGraphVariableFloat(CIRCLING_MAX_ANG_GV, circlingAngleMax)) {
					return _RescaleCircleAngle(a_circleMult, circlingAngleMin, circlingAngleMax);
				}
			}
		}

		return _RescaleCircleAngle(a_circleMult, a_minAnlge, a_maxAngle);
	}

	float CircleAngleHook2::GetMinCircleAngle()
	{
		auto me = CombatAI__get_me();
		if (me && me->combatController && me->combatController->combatStyle) {
			bool enableCircling;
			if (me->GetGraphVariableBool(ENABLE_CIRCLING_GV, enableCircling) && enableCircling && IsMeleeOnly(me)) {
				float circlingAngleMin;
				if (me->GetGraphVariableFloat(CIRCLING_MIN_ANG_GV, circlingAngleMin))
					return circlingAngleMin;
			}
		}

		auto minCircleAngle = GetGameSettingFloat("fCombatCircleAngleMin");
		if (minCircleAngle.has_value())
			return minCircleAngle.value();

		return 30.f;
	}

	float CircleAngleHook3::GetMaxCircleAngle()
	{
		auto me = CombatAI__get_me();
		if (me && me->combatController && me->combatController->combatStyle) {
			bool enableCircling;
			if (me->GetGraphVariableBool(ENABLE_CIRCLING_GV, enableCircling) && enableCircling && IsMeleeOnly(me)) {
				float circlingAngleMax;
				if (me->GetGraphVariableFloat(CIRCLING_MAX_ANG_GV, circlingAngleMax))
					return circlingAngleMax;
			}
		}

		auto maxCircleAngle = GetGameSettingFloat("fCombatCircleAngleMax");
		if (maxCircleAngle.has_value())
			return maxCircleAngle.value();

		return 90.f;
	}

}