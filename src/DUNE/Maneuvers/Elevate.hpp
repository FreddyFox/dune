//***************************************************************************
// Copyright 2007-2013 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Universidade do Porto. For licensing   *
// terms, conditions, and further information contact lsts@fe.up.pt.        *
//                                                                          *
// European Union Public Licence - EUPL v.1.1 Usage                         *
// Alternatively, this file may be used under the terms of the EUPL,        *
// Version 1.1 only (the "Licence"), appearing in the file LICENCE.md       *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://www.lsts.pt/dune/licence.                                        *
//***************************************************************************
// Author: Pedro Calado                                                     *
//***************************************************************************

#ifndef DUNE_MANEUVERS_ELEVATE_HPP_INCLUDED_
#define DUNE_MANEUVERS_ELEVATE_HPP_INCLUDED_

// DUNE headers.
#include <DUNE/IMC.hpp>
#include <DUNE/Maneuvers/Maneuver.hpp>

namespace DUNE
{
  namespace Maneuvers
  {
    // Export DLL Symbol.
    class DUNE_DLL_SYM Elevate;

    //! Class to control elevator behavior
    class Elevate
    {
    public:
      //! Default constructor.
      //! @param[in] maneuver pointer to rows maneuver
      //! @param[in] task pointer to task object (debug and inf)
      //! @param[in] min_radius minimum radius to consider in maneuver
      Elevate(const IMC::Elevator* maneuver, Maneuvers::Maneuver* task,
              float min_radius, float depth_tolerance);

      void
      update(const IMC::EstimatedState* msg);

      void
      updatePathControl(const IMC::PathControlState* msg);

      inline bool
      isDone(void) const
      {
        return (m_els == ST_DONE);
      }

      inline int
      getElevatorDirection(void) const
      {
        return m_dir;
      }

      float
      getVerticalError(float depth, float altitude)
      {
        if (m_elevator.end_z_units == IMC::Z_ALTITUDE)
          return std::fabs(altitude - m_elevator.end_z);
        else
          return std::fabs(depth - m_elevator.end_z);
      }

      float
      getVerticalError(const IMC::EstimatedState* msg)
      {
        return getVerticalError(msg->depth, msg->alt);
      }

    private:
      enum ElevateState
      {
        //! Initial state
        ST_INITIAL,
        //! Going to location
        ST_GO_TO,
        //! Started loitering
        ST_LOITER,
        //! Loitering up or down
        ST_HELICOID,
        //! Done
        ST_DONE
      };

      void
      computeElevatorDirection(const IMC::EstimatedState* msg);

      void
      startHelicoid(const IMC::EstimatedState* msg);

      inline bool
      useCurr(void) const
      {
        return (m_elevator.flags & IMC::Elevator::FLG_CURR_POS);
      }

      unsigned
      getLoiterDirection(const IMC::EstimatedState* msg) const
      {
        // If we are going down then loiter counter-clockwise
        if (m_elevator.end_z_units == IMC::Z_DEPTH)
        {
          if (msg->depth < m_elevator.end_z)
            return IMC::DesiredPath::FL_CCLOCKW;
        }
        else if (msg->alt > m_elevator.end_z)
        {
          return IMC::DesiredPath::FL_CCLOCKW;
        }

        return 0;
      }

      //! Pointer to task
      Maneuvers::Maneuver* m_task;
      //! DesiredPath message
      IMC::DesiredPath m_path;
      //! Elevator maneuver
      IMC::Elevator m_elevator;
      //! Current state of the state machine
      ElevateState m_els;
      //! Direction of the elevator
      int m_dir;
      //! Depth tolerance when elevating towards a new depth
      float m_depth_tolerance;
    };
  }
}

#endif
