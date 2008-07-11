/*---------------------------------------------------------------------------*\
                          ____  _ _ __ _ __  ___ _ _
                         |_ / || | '_ \ '_ \/ -_) '_|
                         /__|\_, | .__/ .__/\___|_|
                             |__/|_|  |_|
\*---------------------------------------------------------------------------*/

#ifndef ZMART_KEYRINGCALLBACKS_H
#define ZMART_KEYRINGCALLBACKS_H

#include <stdlib.h>
#include <iostream>
#include <boost/format.hpp>

#include "zypp/base/Logger.h"
#include "zypp/ZYppCallbacks.h"
#include "zypp/Pathname.h"
#include "zypp/KeyRing.h"
#include "zypp/Digest.h"

#include "utils/prompt.h"

///////////////////////////////////////////////////////////////////
namespace zypp {
/////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // KeyRingReceive
    ///////////////////////////////////////////////////////////////////
    struct KeyRingReceive : public zypp::callback::ReceiveReport<zypp::KeyRingReport>
    {
      KeyRingReceive() : _gopts(Zypper::instance()->globalOpts()) {}

      virtual bool askUserToAcceptUnsignedFile( const std::string &file )
      {
        if (_gopts.no_gpg_checks)
        {
          MIL << "Accepting unsigned file (" << file << ")" << std::endl;
          Zypper::instance()->out().warning(boost::str(
            boost::format(_("Accepting an unsigned file %s.")) % file),
            Out::HIGH);
          return true;
        }

        std::string question = boost::str(boost::format(
            // TranslatorExplanation: speaking of a file
            _("%s is unsigned, continue?")) % file);
        return read_bool_answer(PROMPT_YN_GPG_UNSIGNED_FILE_ACCEPT, question, false);
      }

      virtual bool askUserToImportKey( const PublicKey &key )
      {
        //this is because only root have access to rpm db where is keys stored
        if ( geteuid() != 0 && !_gopts.changedRoot)
          return false;

        std::string question = boost::str(boost::format(
            _("Import key %s to trusted keyring?")) % key.id());
        return read_bool_answer(PROMPT_YN_GPG_KEY_IMPORT_TRUSTED, question, false);
      }

      virtual bool askUserToAcceptUnknownKey( const std::string &file, const std::string &id )
      {
        if (_gopts.no_gpg_checks)
        {
          MIL << "Accepting file signed with an unknown key (" << file << "," << id << ")" << std::endl;
          Zypper::instance()->out().warning(boost::str(boost::format(
              _("Accepting file %s signed with an unknown key %s."))
              % file % id));
          return true;
        }

        std::string question = boost::str(boost::format(
            // TranslatorExplanation: speaking of a file
            _("%s is signed with an unknown key %s. Continue?")) % file % id);
        return read_bool_answer(PROMPT_YN_GPG_UNKNOWN_KEY_ACCEPT, question, false);
      }

      virtual bool askUserToTrustKey( const PublicKey &key )
      {
	const std::string& keyid = key.id(), keyname = key.name(),
	  fingerprint = key.fingerprint();

        if (_gopts.no_gpg_checks)
        {
          MIL << boost::format("Automatically trusting key id %s, %s, fingerprint %s")
              % keyid % keyname % fingerprint << std::endl;
          Zypper::instance()->out().info(boost::str(boost::format(
              _("Automatically trusting key id %s, %s, fingerprint %s"))
              % keyid % keyname % fingerprint));
          return true;
        }

        std::string question = boost::str(boost::format(
	    _("Do you want to trust key id %s, %s, fingerprint %s"))
	    % keyid % keyname % fingerprint);
        return read_bool_answer(PROMPT_YN_GPG_KEY_TRUST, question, false);
      }

      virtual bool askUserToAcceptVerificationFailed( const std::string &file,const PublicKey &key )
      {
	const std::string& keyid = key.id(), keyname = key.name(),
	  fingerprint = key.fingerprint();

        if (_gopts.no_gpg_checks)
        {
          MIL << boost::format(
              "Ignoring failed signature verification for %s"
              " with public key id %s, %s, fingerprint %s")
              % file % keyid % keyname % fingerprint << std::endl;
          Zypper::instance()->out().warning(boost::str(boost::format(
              _("Ignoring failed signature verification for %s"
                " with public key id %s, %s, fingerprint %s!\n"
                "Double-check this is not caused by some malicious"
                " changes in the file!"))
              %file % keyid % keyname % fingerprint),
              Out::QUIET);
          return true;
        }

        std::string question = boost::str(boost::format(
            _("Signature verification failed for %s"
              " with public key id %s, %s, fingerprint %s.\n"
              "Warning: This might be caused by a malicious change in the file!\n"
              "Continuing is risky! Continue anyway?"))
            % file % keyid % keyname % fingerprint);
        return read_bool_answer(PROMPT_YN_GPG_CHECK_FAILED_IGNORE, question, false);
      }

    private:
      const GlobalOptions & _gopts;
    };

    struct DigestReceive : public zypp::callback::ReceiveReport<zypp::DigestReport>
    {
      DigestReceive() : _gopts(Zypper::instance()->globalOpts()) {}

      virtual bool askUserToAcceptNoDigest( const zypp::Pathname &file )
      {
	std::string question = boost::str(boost::format(
	    _("No digest for file %s.")) % file) + " " + _("Continue?");
        return read_bool_answer(PROMPT_GPG_NO_DIGEST_ACCEPT, question, _gopts.no_gpg_checks);
      }

      virtual bool askUserToAccepUnknownDigest( const Pathname &file, const std::string &name )
      {
        std::string question = boost::str(boost::format(
            _("Unknown digest %s for file %s.")) %name % file) + " " +
            _("Continue?");
        return read_bool_answer(PROMPT_GPG_UNKNOWN_DIGEST_ACCEPT, question, _gopts.no_gpg_checks);
      }

      virtual bool askUserToAcceptWrongDigest( const Pathname &file, const std::string &requested, const std::string &found )
      {
        if (_gopts.no_gpg_checks)
        {
          WAR << boost::format(
              "Ignoring failed digest verification for %s (expected %s, found %s).")
              % file % requested % found << std::endl;
          Zypper::instance()->out().warning(boost::str(boost::format(
              _("Ignoring failed digest verification for %s (expected %s, found %s)."))
              % file % requested % found),
              Out::QUIET);
          return true;
        }

	std::string question = boost::str(boost::format(
	    _("Digest verification failed for %s. Expected %s, found %s."))
	    % file.basename() % requested % found) + " " + _("Continue?");
        return read_bool_answer(PROMPT_GPG_WRONG_DIGEST_ACCEPT, question, false);
      }

    private:
      const GlobalOptions & _gopts;
    };

    ///////////////////////////////////////////////////////////////////
}; // namespace zypp
///////////////////////////////////////////////////////////////////

class KeyRingCallbacks {

  private:
    zypp::KeyRingReceive _keyRingReport;

  public:
    KeyRingCallbacks()
    {
      _keyRingReport.connect();
    }

    ~KeyRingCallbacks()
    {
      _keyRingReport.disconnect();
    }

};

class DigestCallbacks {

  private:
    zypp::DigestReceive _digestReport;

  public:
    DigestCallbacks()
    {
      _digestReport.connect();
    }

    ~DigestCallbacks()
    {
      _digestReport.disconnect();
    }

};


#endif // ZMD_BACKEND_KEYRINGCALLBACKS_H
// Local Variables:
// c-basic-offset: 2
// End:
