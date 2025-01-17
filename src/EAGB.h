#ifndef __EAGB_h__
#define __EAGB_h__

#include "constants.h"
#include "typedefs.h"
#include "profiling.h"
#include "utils.h"

#include "CHeB.h"


class BaseStar;
class CHeB;

class EAGB: virtual public BaseStar, public CHeB {

public:

    EAGB(const BaseStar &p_BaseStar, const bool p_Initialise = true) : BaseStar(p_BaseStar), CHeB(p_BaseStar, false) {
        m_StellarType = STELLAR_TYPE::EARLY_ASYMPTOTIC_GIANT_BRANCH;                                                                                                    // Set stellar type
        if (p_Initialise) Initialise();                                                                                                                                 // Initialise if required
    }

    EAGB* Clone(const OBJECT_PERSISTENCE p_Persistence, const bool p_Initialise = true) {
        EAGB* clone = new EAGB(*this, p_Initialise); 
        clone->SetPersistence(p_Persistence); 
        return clone; 
    }

    static EAGB* Clone(EAGB& p_Star, const OBJECT_PERSISTENCE p_Persistence, const bool p_Initialise = true) {
        EAGB* clone = new EAGB(p_Star, p_Initialise); 
        clone->SetPersistence(p_Persistence); 
        return clone; 
    }


    // member functions
    static double CalculateRadiusOnPhase_Static(const double      p_Mass,
                                                const double      p_Luminosity,
                                                const double      p_MHeF,
                                                const DBL_VECTOR &p_BnCoefficients);


protected:

    void Initialise() {
        CalculateTimescales();                                                                                                                                          // Initialise timescales
        m_Age = m_Timescales[static_cast<int>(TIMESCALE::tHeI)] + m_Timescales[static_cast<int>(TIMESCALE::tHe)];                                                       // Set age appropriately
    }


    // member functions - alphabetically
    double          CalculateCOCoreMassAtPhaseEnd() const                                           { return m_GBParams[static_cast<int>(GBP::McDU)]; }                 // McCO(EAGB) = McDU at phase end (Hurley et al. 2000, section 5.4)
    double          CalculateCOCoreMassOnPhase(const double p_Time) const;
    double          CalculateCOCoreMassOnPhase() const                                              { return CalculateCOCoreMassOnPhase(m_Age); }                       // Use class member variables

    double          CalculateCoreMassAtPhaseEnd() const                                             { return m_GBParams[static_cast<int>(GBP::McDU)]; }                 // Mc(EAGB) = McDU at phase end (Hurley et al. 2000, section 5.4)
    double          CalculateCoreMassOnPhase() const                                                { return m_GBParams[static_cast<int>(GBP::McBAGB)]; }               // Mc(EAGB) = McHe(EAGB) = McBAGB on phase (Hurley et al. 2000, section 5.4)

    double          CalculateCriticalMassRatioHurleyHjellmingWebbink() const                        { return GiantBranch::CalculateCriticalMassRatioHurleyHjellmingWebbink(); }

    double          CalculateHeCoreMassAtPhaseEnd() const                                           { return CalculateHeCoreMassOnPhase(); }                            // Same as on phase
    double          CalculateHeCoreMassOnPhase() const                                              { return m_HeCoreMass; }                                            // NO-OP

    double          CalculateInitialSupernovaMass() const                                           { return m_GBParams[static_cast<int>(GBP::McBAGB)]; }               // For EAGB & TPAGB we use the mass at Base Asymptotic Giant Branch to determine SN type

    double          CalculateLambdaNanjingStarTrack(const double p_Mass, const double p_Metallicity) const;
    double          CalculateLambdaNanjingEnhanced(const int p_MassInd, const int p_Zind) const;

    double          CalculateLifetimeTo2ndDredgeUp(const double p_Tinf1_FAGB, const double p_Tinf2_FAGB) const;

    double          CalculateLuminosityAtPhaseEnd(const double p_CoreMass) const                    { return CalculateLuminosityOnPhase(p_CoreMass); }                  // Same as on phase
    double          CalculateLuminosityAtPhaseEnd() const                                           { return CalculateLuminosityAtPhaseEnd(m_COCoreMass); }             // Use class member variables
    double          CalculateLuminosityOnPhase(const double p_CoreMass) const;
    double          CalculateLuminosityOnPhase() const                                              { return CalculateLuminosityOnPhase(m_COCoreMass); }

    double          CalculateMassLossRateHurley();

    double          CalculateRadialExtentConvectiveEnvelope() const                                 { return GiantBranch::CalculateRadialExtentConvectiveEnvelope(); }  // Skip CHeB

    double          CalculateRadiusAtPhaseEnd(const double p_Mass, const double p_Luminosity) const { return CalculateRadiusOnPhase(p_Mass, p_Luminosity); }            // Same as on phase
    double          CalculateRadiusAtPhaseEnd() const                                               { return CalculateRadiusAtPhaseEnd(m_Mass, m_Luminosity); }         // Use class member variables
    double          CalculateRadiusOnPhase(const double p_Mass, const double p_Luminosity) const    { return CalculateRadiusOnPhase_Static(p_Mass, p_Luminosity, m_MassCutoffs[static_cast<int>(MASS_CUTOFF::MHeF)], m_BnCoefficients); }
    double          CalculateRadiusOnPhase() const                                                  { return CalculateRadiusOnPhase(m_Mass, m_Luminosity); }            // Use class member variables

    double          CalculateRemnantLuminosity() const;
    double          CalculateRemnantRadius() const;

    double          CalculateTauAtPhaseEnd() const                                                  { return m_Tau; }                                                   // NO-OP
    double          CalculateTauOnPhase() const                                                     { return m_Tau; }                                                   // NO-OP

    void            CalculateTimescales(const double p_Mass, DBL_VECTOR &p_Timescales);
    void            CalculateTimescales()                                                           { CalculateTimescales(m_Mass0, m_Timescales); }                     // Use class member variables

    double          ChooseTimestep(const double p_Time) const;

    ENVELOPE        DetermineEnvelopeType() const                                                   { return ENVELOPE::CONVECTIVE; }                                    // Always CONVECTIVE

    STELLAR_TYPE    EvolveToNextPhase();

    bool            IsEndOfPhase() const                                                            { return !ShouldEvolveOnPhase(); }                                  // Phase ends when age at or after DU timescale, and no TPAGB
    bool            IsSupernova() const;

    STELLAR_TYPE    ResolveEnvelopeLoss(bool p_Force = false);
    void            ResolveHeliumFlash() {  }                                                                                                                           // NO-OP
    STELLAR_TYPE    ResolveSkippedPhase()                                                           { return m_StellarType; }                                           // NO-OP

    bool            ShouldEvolveOnPhase() const;
    bool            ShouldSkipPhase() const;

};

#endif // __EAGB_h__
