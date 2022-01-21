/* * The modules are executed in the order as they are specified. Loading * mod_auth AFTER mod_fastcgi might disable authentication for fastcgi * backends (if check-local is disabled). * * As auth should be done first, move it before all executing modules (like * proxy, fastcgi, scgi and cgi). * * rewrites, redirects and access should be first, followed by auth and * the docroot plugins. * * Afterwards the external handlers like fastcgi, cgi, scgi and proxy and * at the bottom the post-processing plugins like mod_accesslog.
*/

PLUGIN_INIT(mod_rewrite) /* PCRE */
PLUGIN_INIT(mod_redirect) /* PCRE */
PLUGIN_INIT(mod_alias)

PLUGIN_INIT(mod_extforward)

PLUGIN_INIT(mod_access)
PLUGIN_INIT(mod_auth) /* CRYPT LDAP LBER */
PLUGIN_INIT(mod_authn_file)

PLUGIN_INIT(mod_setenv)

#ifdef HAVE_LUA
PLUGIN_INIT(mod_magnet) /* LUA */
#endif
PLUGIN_INIT(mod_flv_streaming)

/* * indexfile must come before dirlisting for dirlisting not to override */

PLUGIN_INIT(mod_indexfile)
PLUGIN_INIT(mod_userdir)
PLUGIN_INIT(mod_dirlisting)

PLUGIN_INIT(mod_status)

PLUGIN_INIT(mod_simple_vhost)
//PLUGIN_INIT(mod_evhost)
#ifdef HAVE_MYSQL
//PLUGIN_INIT(mod_mysql_vhost) /* MySQL */
#endif

PLUGIN_INIT(mod_secdownload)

PLUGIN_INIT(mod_cgi)
PLUGIN_INIT(mod_fastcgi)
PLUGIN_INIT(mod_scgi)
PLUGIN_INIT(mod_ssi) /* PCRE */
PLUGIN_INIT(mod_deflate)
PLUGIN_INIT(mod_proxy)

/* staticfile must come after cgi/ssi/et al. */
PLUGIN_INIT(mod_staticfile)

#ifdef HAVE_LUA
//PLUGIN_INIT(mod_cml) /* MEMCACHE LUA LUALIB */
#endif

//PLUGIN_INIT(mod_trigger_b4_dl) /* PCRE */

//PLUGIN_INIT(mod_webdav) /* XML2 SQLITE3 UUID */

/* post-processing modules */
PLUGIN_INIT(mod_evasive)
//PLUGIN_INIT(mod_compress) /* Z BZ2 */
PLUGIN_INIT(mod_usertrack)
PLUGIN_INIT(mod_expire)
//PLUGIN_INIT(mod_rrdtool)
PLUGIN_INIT(mod_accesslog)
