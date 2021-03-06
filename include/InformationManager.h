/* -------------------------------------------------------------------------- */
/* Copyright 2002-2020, OpenNebula Project, OpenNebula Systems                */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#ifndef INFORMATION_MANAGER_H_
#define INFORMATION_MANAGER_H_

#include "DriverManager.h"
#include "ActionManager.h"
#include "OpenNebulaMessages.h"

class HostPool;
class Host;
class VirtualMachinePool;

class InformationManager :
    public DriverManager<OpenNebulaMessages, Driver<OpenNebulaMessages>>,
    public ActionListener
{
public:

    InformationManager(
        HostPool * _hpool,
        VirtualMachinePool * _vmpool,
        const string& mad_location)
            : DriverManager(mad_location)
            , hpool(_hpool)
            , vmpool(_vmpool)
    {
        am.addListener(this);
    }

    ~InformationManager() = default;

    /**
     *  This functions starts the associated listener thread, and creates a
     *  new thread for the Information Manager. This thread will wait in
     *  an action loop till it receives ACTION_FINALIZE.
     *    @return 0 on success.
     */
    int start();

    /**
     *  Join the action loop thread
     */
    void join_thread()
    {
        return im_thread.join();
    };

    /**
     *
     */
    void finalize()
    {
        am.finalize();
    };

    /**
     *  Sends a STOPMONITR command to the associated driver and host
     *    @param hid the host id
     *    @param name of the host
     *    @param im_mad the driver name
     */
    void stop_monitor(int hid, const string& name, const string& im_mad);

    /**
     *  Starts the monitor process on the host
     *    @param host to monitor
     *    @param update_remotes to copy the monitor probes to the host
     *    @return 0 on success
     */
    int start_monitor(Host * host, bool update_remotes);

    /**
     *  Send host info to monitor
     */
    void update_host(Host *host);

    /**
     *  Send host delete message to monitor
     */
    void delete_host(int hid);

protected:
    /**
     *  Received undefined message -> print error
     */
    static void _undefined(unique_ptr<Message<OpenNebulaMessages>> msg);

    /**
     *  Message HOST_STATE update from monitor
     */
    void _host_state(unique_ptr<Message<OpenNebulaMessages>> msg);

    /**
     *  Message HOST_SYSTEM update from monitor
     */
    void _host_system(unique_ptr<Message<OpenNebulaMessages>> msg);

    /**
     *  Message VM_STATE from monitor
     */
    void _vm_state(unique_ptr<Message<OpenNebulaMessages>> msg);

private:
    /**
     *  Thread for the Information Manager
     */
    std::thread     im_thread;

    /**
     *  Pointer to the Host Pool
     */
    HostPool *      hpool;

    /**
     *  Pointer to the Host Pool
     */
    VirtualMachinePool * vmpool;

    /**
     *  Action engine for the Manager
     */
    ActionManager   am;

    /**
     *  Default timeout to wait for Information Driver (monitord)
     */
    static const int drivers_timeout = 10;

    // ------------------------------------------------------------------------
    // ActioListener Interface
    // ------------------------------------------------------------------------
    void finalize_action(const ActionRequest& ar) override
    {
        NebulaLog::log("InM",Log::INFO,"Stopping Information Manager...");

        stop(drivers_timeout);
    };
};

#endif /*VIRTUAL_MACHINE_MANAGER_H*/

